//
// Created by Nico Schäfer on 4/25/17.
//

#include "../include/joda/queryexecution/QueryPlan.h"

#include <glog/logging.h>
#include <joda/concurrency/ThreadManager.h>
#include <joda/document/TemporaryOrigin.h>
#include <joda/fs/DirectoryRegister.h>
#include <joda/join/FileJoinManager.h>
#include <joda/join/JoinParser.h>
#include <joda/join/MemoryJoinManager.h>
#include <joda/misc/Benchmark.h>
#include <joda/misc/MemoryUtility.h>
#include <joda/misc/Timer.h>
#include <ctime>
#include <numeric>
#include "../../export/include/joda/export/FileExport.h"
#include "../../export/include/joda/export/IExportDestination.h"
#include "../../export/include/joda/export/JoinExport.h"
#include "../../export/include/joda/export/StorageExport.h"
#include "../../storage/collection/include/joda/storage/collection/StorageCollection.h"
#include "QueryThread.h"
#include "executor/CacheExecutor.h"
#include "executor/ConstExecutor.h"
#include "executor/DefaultExecutor.h"

unsigned long QueryPlan::executeQuery(Benchmark* benchmark) {
  if (skip) {
    return resultID;
  }
  bool bench = benchmark != nullptr;
  if (bench) {
    std::time_t t = std::time(nullptr);
    benchmark->addValue("Time", t);
    std::string t_str = std::ctime(&t);
    benchmark->addValue("Pretty Time", t_str);
  }
  /*
   * Initialize QueryThread
   */
  const auto& deleteVar = q->getDelete();
  const auto& loadJoin = q->getLoadJoinManager();
  const auto& importSources = q->getImportSources();
  LOG(INFO) << "Initializing QueryThread";
  QueryThreadConfig execConfig(q);
  execConfig.bench = benchmark;
  execConfig.addExecutor(std::make_unique<DefaultExecutor>());
  execConfig.addExecutor(std::make_unique<ConstExecutor>());
  if (config::queryCache) {
    execConfig.addExecutor(std::make_unique<CacheExecutor>(*q));
  }

  execConfig.tmpdir =
      joda::filesystem::DirectoryRegister::getInstance().getUniqueDir();
  joda::filesystem::DirectoryRegister::getInstance().registerDirectory(
      execConfig.tmpdir, false);

  /*
   * Parse
   */
  Timer time;

  auto& storageCollection = StorageCollection::getInstance();
  if (!importSources.empty()) {
    LOG(INFO) << "Starting parsing";
    unsigned long long estimation =
        std::accumulate(importSources.begin(), importSources.end(), 0l,
                        [](long est, const auto& source) {
                          return est + source->estimatedSize();
                        });
    storageCollection.ensureSpace(estimation);
    parser.parse(importSources, load);

    time.stop();
    if (bench) {
      benchmark->addValue("Parsing Threads", parser.getMaxThreads());
    }
    if (bench) {
      benchmark->addValue(Benchmark::RUNTIME, "Parsing",
                          time.durationSeconds());
    }
  }

  /*
   * Join load
   */

  if (loadJoin != nullptr) {
    time.start();
    if (!config::storeJson) {
      auto* fjm = dynamic_cast<FileJoinManager*>(loadJoin.get());
      CHECK(fjm != nullptr)
          << "When in nostore mode, joinmanager should always be file-based";
      JoinParser jp;
      jp.parse(*fjm, load);

    } else {
      auto* fjm = dynamic_cast<MemoryJoinManager*>(loadJoin.get());
      CHECK(fjm != nullptr)
          << "When in store mode, joinmanager should always be memory-based";
      fjm->loadJoin(load);
    }
    storageCollection.stopJoin(*loadJoin);  // Delete JoinManager
    time.stop();
    if (bench) {
      benchmark->addValue(Benchmark::RUNTIME, "Load Join",
                          time.durationSeconds());
    }
  }

  if (load->size() == 0) {
    addGenericBenchmarkInformation(benchmark);
    return JODA_STORE_EMPTY_RS_ID;
  }

  /*
   * Execute
   */

  time.start();

  if (!(q->isDefault())) {
    if (bench) {
      benchmark->addValue("Threads", maxThreads);
    }
    // Execute actual query
    auto& exportDestination = q->getExportDestination();
    if (exportDestination ==
        nullptr) {  // LOAD X [...] has to be stored in temporary resultset
      auto s = std::make_shared<JSONStorage>(JODA_TEMPORARY_STORAGE_NAME);
      exportDestination = std::make_unique<StorageExport>(s);
      execConfig.tmpdir = s->getRegtmpdir();
    } else {
      auto* storeDestination =
          dynamic_cast<StorageExport*>(exportDestination.get());
      if (storeDestination != nullptr) {
        execConfig.tmpdir = storeDestination->getStore()->getRegtmpdir();
      } else {
        execConfig.tmpdir = load->getRegtmpdir();
      }
    }

    // Estimate space required for query
    double factor = 1.0;
    if (q->canCreateView()) {
      // Creating views is estimated with 0.1 of normal space
      factor = 0.1;
    }

    // Ensure = old parsed size (if evicted)
    unsigned long long toEnsure = load->parsedSize();
    auto estimated = load->estimatedSize();
    if (toEnsure > estimated) {
      // If past size is larger than the current size,
      // we have to reparse the difference of the original set
      toEnsure -= estimated;
    } else {
      // If the current size is larger or equal, everything is already parsed
      toEnsure = 0;
    }
    // Ensure the size for reparsing the LOAD dataset + for creating a new one
    storageCollection.ensureSpace(
        toEnsure + static_cast<unsigned long long>(factor * load->parsedSize()),
        load);

    if (q->hasAggregators()) {
      execConfig.aggQueue = aggregatorQueue.get();
    }
    std::unique_ptr<IOThreadPool<QueryThread>> exec;
    if (q->hasAggregators()) {
      exec = std::make_unique<IOThreadPool<QueryThread>>(
          loadQueue.get(), nullptr, maxThreads, execConfig);
    } else {
      exec = std::make_unique<IOThreadPool<QueryThread>>(
          loadQueue.get(), storeQueue.get(), maxThreads, execConfig);
    }
    load->getDocumentsQueue(loadQueue.get());
    if (q->hasAggregators()) {
      auto aggCont = aggregate(q, aggregatorQueue.get(), benchmark);
      auto ptok = std::make_unique<JsonContainerQueue::queue_t::ptok_t>(
          storeQueue->queue);
      storeQueue->registerProducer();
      storeQueue->send(*ptok, std::move(aggCont));
      storeQueue->producerFinished();
      storeQueue->unregisterProducer();
    }

    if (exportDestination != nullptr) {
      exportDestination->consume(*storeQueue);
      if (bench) {
        auto t = exportDestination->getTimer();
        benchmark->addValue(Benchmark::RUNTIME, t.first, t.second);
      }

      // ResultID
      auto* storeDestination =
          dynamic_cast<StorageExport*>(exportDestination.get());
      if (storeDestination != nullptr) {
        storeDestination->getStore()->addQueryString(q->toString()+";");
        resultID = storeDestination->getTemporaryResultID();
        /*
         * Check for delta dependencies
         */
        if (config::enable_views) {
          for (const auto& outCont :
               storeDestination->getStore()->getContainer()) {
            for (const auto& inCont : load->getContainer()) {
              if (outCont->isBaseContainer(inCont.get())) {
                storageCollection.addDependency(storeDestination->getStore(),
                                                load);
                goto foundDependency;
              }
            }
          }
        foundDependency:;
        }
      }
    }
  } else {
    resultID = storageCollection.addTemporaryStorage(load);
  }

  time.stop();
  if (bench) {
    benchmark->addValue(Benchmark::RUNTIME, "Evaluation",
                        time.durationSeconds());
  }

  // Delete
  if (!deleteVar.empty()) {
    LOG(INFO) << "Deleting data source " << deleteVar;
    storageCollection.removeStorage(deleteVar);
  }

  if (bench && resultID >= JODA_STORE_VALID_ID_START) {
    auto tmp = storageCollection.getStorage(resultID);
    assert(tmp != nullptr);
    benchmark->addValue("Result Size", tmp->size());
    benchmark->addValue("#Container", tmp->contSize());
  }
  addGenericBenchmarkInformation(benchmark);

  return resultID;
}

std::unique_ptr<JSONContainer> QueryPlan::aggregate(
    std::shared_ptr<joda::query::Query>& q,
    joda::query::AggregatorQueue::queue_t* queue, Benchmark* benchmark) {
  DCHECK(queue != nullptr);
  Timer agg_timer;
  auto aggCtok =
      std::make_unique<joda::query::AggregatorQueue::queue_t::ctok_t>(
          queue->queue);
  while (!queue->isFinished()) {
    std::unique_ptr<joda::query::IAggregator> agg = nullptr;
    queue->retrieve(*aggCtok, agg);
    if (agg == nullptr) {
      continue;
    }

    for (auto& a : q->getAggregators()) {
      auto aStr = a->toString();
      if (agg->toString() == aStr) {
        a->merge(agg.get());
        agg = nullptr;
        break;
      }
    }
  }
  LOG(INFO) << "Finished merging aggregators";

  /*
   * Make object
   */
  RJMemoryPoolAlloc alloc(1024 * 1024);  // 1mb blocks

  auto doc = std::make_unique<RJDocument>(&alloc);
  doc->SetObject();
  for (auto&& a : q->getAggregators()) {
    auto val = a->terminate(alloc);
    RJPointer p(a->getDestPointer().c_str());
    p.Set(*doc, val);
    DCHECK(val.IsNull()) << "Document was copied, not moved";
  }

  auto cont = std::make_unique<JSONContainer>();
  auto tmpDoc = std::make_unique<RJDocument>(cont->getAlloc());
  tmpDoc->CopyFrom(*doc, *cont->getAlloc());
  cont->insertDoc(std::move(tmpDoc), std::make_unique<TemporaryOrigin>());
  cont->finalize();

  agg_timer.stop();
  if (benchmark != nullptr) {
    benchmark->addValue(Benchmark::RUNTIME, "AggMerge",
                        agg_timer.durationSeconds());
  }
  return cont;
}

QueryPlan::QueryPlan(const std::shared_ptr<joda::query::Query>& q)
    : resultID(JODA_STORE_EXTERNAL_RS_ID),
      maxThreads(g_ThreadManagerInstance.getMaxThreads()),
      q(q),
      loadQueue(JsonContainerRefQueue::getQueue(10, 1)),
      storeQueue(JsonContainerQueue::getQueue(10, maxThreads + 1)),
      aggregatorQueue(joda::query::AggregatorQueue::getQueue(
          maxThreads * (q->getAggregators().size()), maxThreads)) {
  auto loadVar = q->getLoad();
  auto deleteVar = q->getDelete();
  auto storeJoin = q->getStoreJoinManager();
  auto loadJoin = q->getLoadJoinManager();

  // LOAD
  CHECK(!loadVar.empty())
      << "Load var is not optional, this should not be possible to happen";
  load = StorageCollection::getInstance().getStorage(loadVar);
  if (load == nullptr && q->getImportSources().empty() && loadJoin == nullptr) {
    LOG(ERROR) << "Datasource " << loadVar << " not found.";
    skip = true;
    resultID = JODA_STORE_SKIPPED_QUERY_ID;
    return;
  }
  if (load != nullptr && !q->getImportSources().empty()) {
    LOG(INFO) << "Datasource " << loadVar
              << " already populated, appending file-contents";
  }

  if (load == nullptr) {
    load = StorageCollection::getInstance().getOrAddStorage(loadVar);
  }

  if(!q->getImportSources().empty() || loadJoin != nullptr) {
    load->addQueryString(q->toString()+";");
  }

  CHECK(load != nullptr);
  std::string storeVar;
  auto& exp = q->getExportDestination();
  if (exp != nullptr) {
    auto* storeExp = dynamic_cast<StorageExport*>(exp.get());
    if (storeExp != nullptr && storeExp->getStorageName() == loadVar) {
      LOG(WARNING) << "Using the same variable for store as for load results "
                      "in duplication of the data";
    }
  }

  parser.setMaxThreads(config::storageRetrievalThreads);
}

bool QueryPlan::hasToParse() const { return !q->getImportSources().empty(); }

ExecutionStats QueryPlan::getStats() const {
  ExecutionStats s{};
  s.parsedDocs = parser.getParsedDocs();
  s.parsedConts = parser.getParsedConts();
  s.evaluatedConts = loadQueue->getStatistics().second;
  if (parser.isParsing() || load == nullptr) {
    s.numConts = 0;
  } else {
    s.numConts = load->contSize();
  }

  return s;
}

void QueryPlan::addGenericBenchmarkInformation(Benchmark* bench) const {
  if (bench == nullptr) {
    return;
  }
  auto procUsage = MemoryUtility::procRamUsage();
  bench->addValue<uint64_t>("RAM Proc", procUsage.getBytes());
  bench->addValue("Pretty RAM Proc", procUsage.getHumanReadable());
  auto storageSize = MemoryUtility::MemorySize(
      StorageCollection::getInstance().estimatedSize());
  bench->addValue<uint64_t>("Estimated Storage Size", storageSize.getBytes());
  bench->addValue("Pretty Estimated Storage Size",
                  storageSize.getHumanReadable());
}
