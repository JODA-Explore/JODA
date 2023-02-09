#include <glog/logging.h>
#include <joda/export/StorageExport.h>
#include <joda/extension/ModuleRegister.h>
#include <joda/misc/MemoryUtility.h>
#include <joda/queryexecution/PipelineQueryPlan.h>
#include <joda/storage/collection/StorageCollection.h>

#include <boost/algorithm/string/join.hpp>

#include "executor/CacheExecutor.h"
#include "executor/AdaptiveIndexExecutor.h"
#include "executor/ConstExecutor.h"
#include "executor/DefaultExecutor.h"
#include "pipeline/optimization/DefaultOptimizations.h"
#include "pipeline/tasks/Compatability.h"

namespace joda::queryexecution {

namespace tasks = pipeline::tasks;

PipelineQueryPlan::PipelineQueryPlan(size_t max_threads)
    : max_threads(max_threads) {
  auto threadPool = std::make_shared<pipeline::ThreadPool>(max_threads);
  p = std::make_unique<pipeline::Pipeline>(threadPool);
}

const std::unique_ptr<pipeline::Pipeline>& PipelineQueryPlan::getPipeline()
    const {
  return p;
}

void PipelineQueryPlan::createPlan(
    const std::vector<std::shared_ptr<joda::query::Query>>& queries) {
  for (size_t i = 0; i < queries.size(); i++) {
    createPlan(queries[i], i == queries.size() - 1);
  }
}

void PipelineQueryPlan::createPlan(
    const std::shared_ptr<const joda::query::Query>& q, bool lastQuery) {
  planning_timer.start();
  current_q = q;
  currentIsStored = false;
  // Load
  planLOAD();

  // Join
  planJOIN();

  // Choose/AS/AGG
  planCHOOSE();
  planAS();
  planAGG();

  // Store
  planSTORE();

  // Resolve remaining requirements
  resolveRequirements();

  commitTasks();
  p->finalize();
  planning_timer.stop();
}

std::vector<pipeline::tasks::PipelineTaskPtr> PipelineQueryPlan::createSubPlan(
    const std::shared_ptr<const joda::query::Query>& q) {
  current_q = q;
  currentIsStored = false;
  // Load
  planLOAD();

  // Join
  planJOIN();

  // Choose/AS/AGG
  planCHOOSE();
  planAS();
  planAGG();

  // Resolve remaining requirements
  resolveRequirements();

  return std::move(taskQueue);
}

std::string PipelineQueryPlan::planLOAD() {
  /*
   * Input
   */
  const auto& importSources = current_q->getImportSources();

  // Estimate source sizes
  for (const auto& source : importSources) {
    auto task = source->getTask();  // Get Import Task
    // TODO Remove splitting of estimated input?
    stream_size += source->estimatedSize();
    ls_size += source->estimatedSize();
    addTask(std::move(task), false);
  }

  stream_size = std::max(static_cast<unsigned long> JSON_CONTAINER_DEFAULT_SIZE,
                         stream_size / (max_threads * 2));
  ls_size = std::max(static_cast<unsigned long> JSON_CONTAINER_DEFAULT_SIZE,
                     ls_size / (max_threads * 2));

  // Resolve parser requirements
  resolveRequirements();

  // Joins
  auto jm = current_q->getLoadJoinManager();
  auto fjm = dynamic_pointer_cast<FileJoinManager>(jm);
  if (fjm != nullptr) {
    addTask(std::make_unique<tasks::join::LoadJoinFilesTask>(fjm));
  }
  auto mjm = dynamic_pointer_cast<MemoryJoinManager>(jm);
  if (mjm != nullptr) {
    addTask(std::make_unique<tasks::join::LoadJoinTask>(mjm));
  }

  // Store

  // Import tasks were added
  auto loadVar = current_q->getLoad();
  if (!loadVar.empty()) {      // Store in a named storage
    if (!taskQueue.empty()) {  // There have been import tasks
                               // Store parse result in storage
      addTask(std::make_unique<
              tasks::PipelineIOTask<tasks::storage::StorageReceiver>>(loadVar));
      currentIsStored = true;
    }
    if (!current_q->isDefault()) {  // Query is following
      // Send containers from storage
      addTask(std::make_unique<
              tasks::PipelineIOTask<tasks::storage::StorageSender>>(loadVar));
      // Note: Receiver + Sender = Buffer. Will be handled by optimizer. Could
      // also already be handled here with more Ifs
    }
  }

  // All other cases => No handling required
  // LOAD X; => No Tasks
  // Temporary set => No storage involved, straight passthrough

  return loadVar;
}

void PipelineQueryPlan::planJOIN() {
  if (!current_q->hasJOIN()) return;

  // Get join executor
  auto joiner = current_q->getJoinExecutor();

  // Add inner join
  addTask(std::make_unique<tasks::join::InnerJoinTask>(joiner));

  // Add outer source
  // - Subquery
  auto subquery = current_q->getSubQuery();
  if (subquery != nullptr) {
    if (subquery->isDefault() &&
        subquery->getImportSources().empty()) {  // Only LOAD X;
      addTask(std::make_unique<tasks::storage::StorageSenderTask>(
          subquery->getLoad()));
    } else {
      // Plan subquery
      PipelineQueryPlan subplanner(max_threads);
      auto tasks = subplanner.createSubPlan(subquery);
      // Add all tasks to queue
      std::move(tasks.begin(), tasks.end(), std::back_inserter(taskQueue));
    }
  } else {  // JoinPartner
    auto joinPartner = current_q->getJoinPartner();
    addTask(std::make_unique<tasks::storage::StorageSenderTask>(joinPartner));
  }

  // Add outer join
  addTask(std::make_unique<tasks::join::OuterJoinTask>(joiner));
}

void PipelineQueryPlan::planCHOOSE() {
  if (!current_q->hasChoose() && !current_q->hasAS()) {
    return;  // Nothing to do
  }

  std::vector<std::unique_ptr<IQueryExecutor>> executors;
  executors.emplace_back(std::make_unique<ConstExecutor>());
  bool constResult = false;
  if (current_q->chooseIsConst(constResult)) {
    // TODO handle const false result? No query evaluation needed
    // Only const executor needed for default query
    addTask(
        std::make_unique<tasks::PipelineIOTask<tasks::choose::ChoosePipeline>>(
            executors, current_q));
  } else {
    // Add remaining executors
    executors.emplace_back(std::make_unique<DefaultExecutor>());
    if (config::queryCache) {
      executors.emplace_back(std::make_unique<CacheExecutor>(*current_q));
    }
    if (config::adaptiveIndex) {
      executors.emplace_back(std::make_unique<AdaptiveIndexExecutor>(*current_q));
    }

    // Add user-supplied indices
    auto moduleExecs =
        extension::ModuleRegister::getInstance().getIndices(*current_q);
    for (auto& exec : moduleExecs) {
      executors.emplace_back(std::move(exec));
    }

    // Create chooser task
    addTask(
        std::make_unique<tasks::PipelineIOTask<tasks::choose::ChoosePipeline>>(
            executors, current_q));
  }
}

void PipelineQueryPlan::planAS() {
  // Default AS is handled by choose requirement
}

void PipelineQueryPlan::planAGG() {
  const auto& aggs = current_q->getAggregators();
  if (aggs.empty()) {
    return;
  }

  if (!current_q->hasAggWindow()) {
    // Add default pipeline
    addTask(std::make_unique<tasks::agg::AggPipelineTask>(current_q));
  } else {
    // Add window aggregation pipeline
    addTask(std::make_unique<tasks::agg::WindowAggTask>(
        current_q, current_q->getAggWindowSize()));
  }
}

std::string PipelineQueryPlan::planSTORE(bool lastQuery) {
  if (taskQueue.empty() || (currentIsStored && current_q->isDefault())) {
    // Query is noop "LOAD X" => handled by createPlan()
    return "";
  }

  auto& exportDestination = current_q->getExportDestination();
  if (exportDestination ==
      nullptr) {  // LOAD X [...] has to be stored in temporary resultset
    if (lastQuery) {
      // Last query stores in temporary resultset
      auto storage = std::make_shared<JSONStorage>("");
      storage->addQueryString(current_q->toString());
      addTask(std::make_unique<tasks::storage::StorageReceiverTask>(
          StorageCollection::getInstance().addTemporaryStorage(storage)));
      return "";
    } else {
      // TODO what to do with queries without output
      // - stream?
      // - Discard previous tasks?
      // - Optimizer removes tasks without outqueue? + Transforms buffer =>
      // Receiver
    }

  } else {
    auto* storeExp = dynamic_cast<StorageExport*>(exportDestination.get());
    if (storeExp != nullptr) {
      // TODO StorageExport could use Storagereceiver if it was moved to other
      // package
      auto storeName = storeExp->getStorageName();
      addTask(std::make_unique<
              tasks::PipelineIOTask<tasks::storage::StorageReceiver>>(
          storeName));
      auto& store = storeExp->getStore();
      if (store != nullptr) {
        store->addQueryString(current_q->toString());
      }
      return storeName;
    } else {
      addTask(exportDestination->getTask());
    }
  }
  return "";
}

unsigned long PipelineQueryPlan::executeAndGetResult(Benchmark* bench) const {
  if (current_q == nullptr) {
    return 0;
  }
  // Log time to benchmark
  if (bench) {
    std::time_t t = std::time(nullptr);
    bench->addValue("Time", t);
    std::string t_str = std::ctime(&t);
    // Remove trailing "\n"
    if (!t_str.empty() && t_str.back() == '\n') {
      t_str.pop_back();
    }
    bench->addValue("Pretty Time", t_str);
    bench->addValue("Threads", max_threads);
  }

#ifndef NDEBUG
  if (bench != nullptr) {
    bench->addValueAt("/UnoptimizedPipeline", p->toJSON());
  }
#endif

  // Optimize
  Timer optimize_timer;
  auto opts = pipeline::optimization::DefaultOptimizations::getRules();
  p->optimize(opts);
  optimize_timer.stop();

  auto taskNames = p->getTaskNameList();
  auto joinedTaskNames =
      boost::algorithm::join(taskNames, ",");  // Join task names
  LOG(INFO) << "Executing pipeline: " << joinedTaskNames;

  Timer execute_timer;
  p->execute();
  execute_timer.stop();

  if (bench != nullptr) {
    bench->addValue(Benchmark::RUNTIME, "Optimization",
                    optimize_timer.durationSeconds());
    bench->addValue(Benchmark::RUNTIME, "Planning",
                    planning_timer.durationSeconds());
    bench->addValue(Benchmark::RUNTIME, "Execution",
                    execute_timer.durationSeconds());
  }

  const auto& p_tasks = p->getTasks();
  unsigned long storageID = JODA_STORE_EXTERNAL_RS_ID;
  if (p_tasks.empty()) {  // LOAD X; query
    auto storage =
        StorageCollection::getInstance().getStorage(current_q->getLoad());
    if (storage == nullptr) {
      storageID = JODA_STORE_EMPTY_RS_ID;
    } else {
      storageID = StorageCollection::getInstance().addTemporaryStorage(storage);
    }
  } else {
    // Get last pipeline task
    const auto& last_task = p_tasks.back();
    // Check if last task is a storagereceiver
    auto storageTask =
        dynamic_cast<tasks::storage::StorageReceiverTask*>(last_task.get());
    if (storageTask != nullptr) {
      // If yes, extract storage
      auto storageData = storageTask->getData();
      auto storageName = std::get<0>(storageData);
      auto tmpStorageID = std::get<2>(storageData);
      if (tmpStorageID != 0) {  // If valid temporary ID, use that
        storageID = tmpStorageID;
      }
      if (!storageName.empty()) {  // If valid name
        auto storage = StorageCollection::getInstance().getStorage(storageName);
        if (storage == nullptr) {  // Check if name exists
          storageID = JODA_STORE_SKIPPED_QUERY_ID;
        } else {
          if (storage->size() == 0) {  // Check if storage is empty
            storageID = JODA_STORE_EMPTY_RS_ID;
          } else {  // Get tmp ID of storage
            storageID =
                StorageCollection::getInstance().addTemporaryStorage(storage);
          }
        }
      }
    }
  }

  if (bench != nullptr) {
    // Log pipeline to benchmark
    bench->addValueAt("/Pipeline", p->toJSON());
    // Log result size to benchmark
    if (storageID >= JODA_STORE_VALID_ID_START) {
      auto tmp = StorageCollection::getInstance().getStorage(storageID);
      bench->addValue("Result Size", tmp->size());
      bench->addValue("#Container", tmp->contSize());
    }
    addGenericBenchmarkInformation(bench);
  }
  return storageID;
}

void PipelineQueryPlan::addTask(pipeline::tasks::PipelineTaskPtr&& task,
                                bool instantResolveRequirement) {
  auto taskID = task->getName();

  // Get required following task
  auto required_taskID = tasks::TaskRequirement::get(taskID);

  // Get required prerequesites
  auto prerequisite_taskID = tasks::TaskPrerequisite::get(taskID);

  // Resolve Prerequisite
  resolvePrerequisite(prerequisite_taskID);

  // Update requirements
  std::erase(unfulfilledRequirements, taskID);
  addedIDs.insert(taskID);

  // Add task to pipeline
  taskQueue.emplace_back(std::move(task));

  if (instantResolveRequirement) {
    // Resolve requirement
    resolveRequirement(required_taskID);
  } else {
    // Add requirement to queue
    if (!required_taskID.empty() &&
        std::find(unfulfilledRequirements.begin(),
                  unfulfilledRequirements.end(),
                  required_taskID) == unfulfilledRequirements.end()) {
      unfulfilledRequirements.emplace_back(std::move(required_taskID));
    }
  }
}

void PipelineQueryPlan::commitTasks() {
  // Resolve outstanding requirements
  resolveRequirements();

  // Add tasks to pipeline
  for (auto&& task : taskQueue) {
    p->addTask(std::move(task));
  }

  taskQueue.clear();

  // Clear ID index
  addedIDs.clear();
}

void PipelineQueryPlan::resolveRequirements() {
  while (!unfulfilledRequirements.empty()) {
    resolveRequirement(unfulfilledRequirements.front());
  }
}

void PipelineQueryPlan::resolveRequirement(const std::string& requirement) {
  if (requirement.empty() || addedIDs.contains(requirement)) {
    return;
  }

  if (requirement == tasks::ID<tasks::load::LSFileOpener>::NAME) {
    // LSFileOpener
    addTask(
        std::make_unique<tasks::PipelineIOTask<tasks::load::LSFileOpener>>());
  } else if (requirement ==
             tasks::ID<tasks::load::UnformattedFileOpener>::NAME) {
    // UnformattedFileOpener
    addTask(std::make_unique<
            tasks::PipelineIOTask<tasks::load::UnformattedFileOpener>>());
  } else if (requirement ==
             tasks::ID<tasks::load::LineSeparatedStreamReader>::NAME) {
    // LS Stream Reader
    addTask(std::make_unique<
            tasks::PipelineIOTask<tasks::load::LineSeparatedStreamReader>>());
  } else if (requirement == tasks::ID<tasks::load::DefaultTextParser>::NAME) {
    switch (config::sim_measure) {
      case config::Sim_Measures::PATH_JACCARD:
        addTask(
            std::make_unique<tasks::load::PathJaccardSimilarityTextParserTask>(
                ls_size));
        break;
      case config::Sim_Measures::ATTRIBUTE_JACCARD:
        addTask(std::make_unique<
                tasks::load::AttributeJaccardSimilarityTextParserTask>(
            ls_size));
        break;
      case config::Sim_Measures::NO_SIMILARITY:
      default:
        addTask(std::make_unique<tasks::load::DefaultTextParserTask>(ls_size));
    }

    ls_size = 0;
  } else if (requirement == tasks::ID<tasks::load::DefaultStreamParser>::NAME) {
    switch (config::sim_measure) {
      case config::Sim_Measures::PATH_JACCARD:
        addTask(std::make_unique<
                tasks::load::PathJaccardSimilarityStreamParserTask>(
            stream_size));
        break;
      case config::Sim_Measures::ATTRIBUTE_JACCARD:
        addTask(std::make_unique<
                tasks::load::AttributeJaccardSimilarityStreamParserTask>(
            stream_size));
        break;
      case config::Sim_Measures::NO_SIMILARITY:
      default:
        addTask(std::make_unique<tasks::load::DefaultStreamParserTask>(
            stream_size));
    }
    stream_size = 0;
  } else if (requirement == tasks::ID<tasks::as::AsPipeline>::NAME) {
    // AsPipeline
    addTask(std::make_unique<tasks::PipelineIOTask<tasks::as::AsPipeline>>(
        current_q));
  } else if (requirement == tasks::ID<tasks::agg::AggMerger>::NAME) {
    // AggMerger
    addTask(std::make_unique<tasks::agg::AggMergerTask>());
  } else if (requirement == tasks::ID<tasks::join::LoadJoinFileParser>::NAME) {
    auto jm =
        dynamic_pointer_cast<FileJoinManager>(current_q->getLoadJoinManager());
    DCHECK(jm != nullptr);
    addTask(std::make_unique<tasks::join::LoadJoinFileParserTask>(jm));
  } else {
    DCHECK(false) << "Unknown requirement " << requirement;
    std::erase(unfulfilledRequirements,
               requirement);  // Erase anyway to prevent infinite loop
  }
}

void PipelineQueryPlan::resolvePrerequisite(const std::string& prerequisite) {
  if (prerequisite.empty() || addedIDs.contains(prerequisite)) {
    return;
  }
  if (prerequisite == tasks::ID<tasks::store::JSONStringifier>::NAME) {
    // JSONStringifier
    addTask(std::make_unique<
            tasks::PipelineIOTask<tasks::store::JSONStringifier>>());
  } else {
    DCHECK(false) << "Unknown prerequisite " << prerequisite;
  }
}

void PipelineQueryPlan::addGenericBenchmarkInformation(Benchmark* bench) const {
  if (bench == nullptr) return;
  auto procUsage = MemoryUtility::procRamUsage();
  bench->addValue<uint64_t>("RAM Proc", procUsage.getBytes());
  bench->addValue("Pretty RAM Proc", procUsage.getHumanReadable());
  auto storageSize = MemoryUtility::MemorySize(
      StorageCollection::getInstance().estimatedSize());
  bench->addValue<uint64_t>("Estimated Storage Size", storageSize.getBytes());
  bench->addValue("Pretty Estimated Storage Size",
                  storageSize.getHumanReadable());
}

}  // namespace joda::queryexecution