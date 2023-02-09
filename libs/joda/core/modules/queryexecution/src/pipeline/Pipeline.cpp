#include "Pipeline.h"

#include <joda/pipelineatomics/PipelineTask.h>

#include <set>

#include "tasks/Compatability.h"

namespace joda::queryexecution::pipeline {

void Pipeline::addTask(tasks::PipelineTaskPtr&& task) {
  auto end = tasks.end();
  addTaskAt(std::move(task), end);
}

tasks::TaskNum Pipeline::addTaskAt(
    tasks::PipelineTaskPtr&& task,
    std::list<tasks::PipelineTaskPtr>::iterator& at) {
  auto num = task->getNum();
  if (num == tasks::PipelineTask::UNSET_NUM) {
    num = ++currNum;
    task->setNum(num);
  }
  auto taskId = task->getName();

  // Create Input queue if required
  if (!task->inputConnected()) {
    // Calculate real previous threads.
    // Always allocating all threads is bad for queue performance
    size_t max_previous_threads = 0;
    for (auto it = tasks.begin(); it != at; ++it) {
      const auto& t = *it;
      auto tid = t->getName();
      if (tasks::TaskCompatability::check(tid, taskId) &&
          !t->outputConnected()) {
        switch (t->getAsyncType()) {
          case tasks::AsyncType::MultiThreaded:
            max_previous_threads += threadPool->getMaxThreads();
            break;
          case tasks::AsyncType::SingleThreaded:
            max_previous_threads += 1;
            break;
          case tasks::AsyncType::Synchronous:
            max_previous_threads += 1;
            break;
          default:
            break;
        }
      }
    }
    max_previous_threads =
        std::min(max_previous_threads, threadPool->getMaxThreads());
    queues.emplace(num, task->createInputQueue(max_previous_threads));
    DLOG(INFO) << "Created input queue with " << max_previous_threads
               << " producers for task " << taskId << "(" << num << ")";
  }

  // Get Raw Pointer
  auto& sptr = queues[num];
  auto* ptr = sptr.get();

  // Iterate all previous tasks and set output if compatible
  if (ptr != nullptr) {
    for (auto it = tasks.begin(); it != at; ++it) {
      const auto& t = *it;
      auto tid = t->getName();
      if (tasks::TaskCompatability::check(tid, taskId) &&
          !t->outputConnected()) {
        t->setOutputQueue(ptr);
        connections.emplace_back(t->getNum(), num);
      }
    }
  }

  // Store Task
  tasks.insert(at, std::move(task));
  return num;
}

std::vector<std::vector<tasks::PipelineTask*>>
Pipeline::splitIndependentPipelines() const {
  std::vector<std::vector<tasks::PipelineTask*>> pipes;
  std::vector<tasks::TaskNum> pipe_num;

  for (auto& t : tasks) {
    if (pipes.empty()) {
      // Create new subpipeline if none exist yet
      pipes.emplace_back();
      pipes.back().emplace_back(t.get());
      pipe_num.emplace_back(t->getNum());
      continue;
    }

    std::vector<std::pair<size_t, size_t>> toMerge;

    bool found = false;
    size_t found_num = 0;
    // For every subpipe
    for (size_t i = 0; i < pipe_num.size(); i++) {
      // Check if task is connected to end
      if (std::ranges::find(connections,
                            std::make_pair(pipe_num[i], t->getNum())) !=
          connections.end()) {
        if (!found) {  // If not already found
          // add to pipeline
          pipes[i].emplace_back(t.get());
          // And update pipe_num
          pipe_num[i] = t->getNum();
          found = true;
          found_num = i;
        } else {  // Else, merge pipelines
          toMerge.emplace_back(found_num, i);
        }
      }
    }

    for (auto& [first, second] : toMerge) {
      // Add pipeline
      pipes[first].insert(pipes[first].end(), pipes[second].begin(),
                          pipes[second].end());
      // Sort pipeline
      std::ranges::sort(pipes[first], [](const auto& a, const auto& b) {
        return a->getNum() < b->getNum();
      });
      // And remove empty
      pipes.erase(pipes.begin() + second);
      pipe_num.erase(pipe_num.begin() + second);
    }

    // If no subpipeline found
    if (!found) {
      // Create new one
      pipes.emplace_back();
      pipes.back().emplace_back(t.get());
      pipe_num.emplace_back(t->getNum());
    }
  }

  return pipes;
}

void Pipeline::execute() {
  auto subpipes = splitIndependentPipelines();
  DLOG_IF(INFO, subpipes.size() > 1)
      << "Pipeline consists of " << subpipes.size()
      << " unconnected pipelines.";
  for (auto& subpipe : subpipes) {
    DLOG(INFO) << "Executing subpipe with " << subpipe.size() << " tasks.";
    
    defaultSchedule(subpipe);
  
    threadPool->wait();
    threadPool->reset();
  }

  threadPool->fillStats(taskTimers, taskStatistics, taskCount);
}

bool Pipeline::hasTasks() const { return !tasks.empty(); }

size_t Pipeline::numTasks() const { return tasks.size(); }

void Pipeline::finalize() {}

RJDocument Pipeline::toJSON() const {
  RJDocument doc(rapidjson::kObjectType);

  doc.AddMember("MaxThreads", threadPool->getMaxThreads(), doc.GetAllocator());

  // Collect inputs/outputs
  // All input tasknumber of task X
  std::unordered_map<tasks::TaskNum, RJValue> inputs;
  // All output tasknumber of task X
  std::unordered_map<tasks::TaskNum, RJValue> outputs;
  for (auto& con : connections) {
    auto o = outputs.find(con.first);
    if (o == outputs.end()) {
      RJValue os(rapidjson::kArrayType);
      os.PushBack(con.second, doc.GetAllocator());
      outputs[con.first] = os;
    } else {
      o->second.PushBack(con.second, doc.GetAllocator());
    }

    auto i = inputs.find(con.second);
    if (i == inputs.end()) {
      RJValue os(rapidjson::kArrayType);
      os.PushBack(con.first, doc.GetAllocator());
      inputs[con.second] = os;
    } else {
      i->second.PushBack(con.first, doc.GetAllocator());
    }
  }

  // Create Tasklists
  RJValue taskList(rapidjson::kArrayType);
  for (auto& task : tasks) {
    auto taskNum = task->getNum();
    auto obj = task->toJSON(doc.GetAllocator());
    // Add timer info if exists
    auto timer_it = taskTimers.find(taskNum);
    if (timer_it != taskTimers.end()) {
      obj.AddMember("Runtime_s", timer_it->second.durationSeconds(),
                    doc.GetAllocator());
      obj.AddMember("Runtime", timer_it->second.toHumanDuration(),
                    doc.GetAllocator());
    }

    // Add Execution Time info if exists
    auto exec_it = taskStatistics.find(taskNum);
    if (exec_it != taskStatistics.end() &&
        exec_it->second.runtime.count() > 0) {
      obj.AddMember("Executiontime_ms", exec_it->second.runtime.count(),
                    doc.GetAllocator());
      obj.AddMember("Executiontime_min_ms", exec_it->second.min_runtime.count(),
                    doc.GetAllocator());
      obj.AddMember("Executiontime_max_ms", exec_it->second.max_runtime.count(),
                    doc.GetAllocator());
    }

    // Add count info if exists
    auto count_it = taskCount.find(taskNum);
    if (count_it != taskCount.end()) {
      obj.AddMember("TaskCount", count_it->second, doc.GetAllocator());
    }

    // Add task to list
    taskList.PushBack(obj, doc.GetAllocator());
  }
  // Add tasklist to doc
  doc.AddMember("Tasks", taskList, doc.GetAllocator());

  // Connection list
  RJValue conList(rapidjson::kArrayType);
  for (auto& [queue_input_for, queuePtr] : queues) {
    auto& queue_output_for = inputs[queue_input_for];
    if (queue_output_for.IsNull()) {
      continue;
    }
    RJValue con(rapidjson::kObjectType);
    con.AddMember("From", queue_output_for, doc.GetAllocator());
    RJValue to(rapidjson::kArrayType);
    to.PushBack(queue_input_for, doc.GetAllocator());
    con.AddMember("To", to, doc.GetAllocator());
    if (queuePtr != nullptr) {
      con.AddMember("Throughput", queuePtr->getThroughput(),
                    doc.GetAllocator());
      con.AddMember("Finished", queuePtr->isFinished(), doc.GetAllocator());
    }

    conList.PushBack(con, doc.GetAllocator());
  }

  doc.AddMember("Connections", conList, doc.GetAllocator());

  return doc;
}

std::vector<std::string> Pipeline::getTaskNameList() const {
  std::vector<std::string> taskNames;
  for (auto& task : tasks) {
    taskNames.push_back(task->getName());
  }
  return taskNames;
}

void Pipeline::optimize(
    std::vector<std::unique_ptr<optimization::OptimizationRule>>&
        optimizations) {
  // TODO Rules that only delete tasks?
  //  TODO Rules that replace non-concecutive tasks
  bool ruleApplied = false;
  do {
    ruleApplied = false;
    for (auto& rule_ptr : optimizations) {
      DCHECK(rule_ptr != nullptr);
      auto& rule = *rule_ptr;
      auto res = rule.optimize(tasks, connections);
      if (res.has_value()) {
        auto& [toReplace, replacementTasks] = res.value();

        DCHECK(!replacementTasks.empty());

        auto& start = toReplace.first;
        DCHECK(start != tasks.end());
        auto start_num = (*start)->getNum();
        auto& end = toReplace.second;
        DCHECK(end != tasks.end());
        auto end_num = (*end)->getNum();

        // Set start number
        (*replacementTasks.begin())->setNum(start_num);

        // Connect input
        auto input_q = queues.find(start_num);
        if (input_q != queues.end()) {
          (*replacementTasks.begin())->setInputQueue(input_q->second.get());
        }

        // Connect output
        auto con =
            std::find_if(connections.begin(), connections.end(),
                         [&](const auto& con) { return con.first == end_num; });
        if (con != connections.end()) {
          auto q_it = queues.find(con->second);
          if (q_it != queues.end()) {
            (*replacementTasks.rbegin())->setOutputQueue(q_it->second.get());
          }
        }

        // Remove tasks
        std::set<tasks::TaskNum> removedNums;
        auto it = start;
        auto pastEnd = end;
        pastEnd++;
        while (it != pastEnd) {
          auto it_num = it->get()->getNum();
          removedNums.insert(it_num);
          if (it != start) {
            queues.erase(it_num);
          }
          it = tasks.erase(it);
        }

        // Add new tasks
        tasks::TaskNum lastNum = 0;
        std::set<tasks::TaskNum> addedNums;
        for (auto& task : replacementTasks) {
          // Insert in place of deleted
          lastNum = addTaskAt(std::move(task), it);
          addedNums.insert(lastNum);
        }
        // Replace connection number of last
        if (con != connections.end()) {
          con->first = lastNum;
        }

        // Remove internal connections
        // TODO does not correctly remove internal connections
        std::erase_if(connections, [&removedNums](const auto& connect) {
          return removedNums.contains(connect.first) &&
                 removedNums.contains(connect.second);
        });

        // Log
        std::stringstream removed;
        for (const auto& num : removedNums) {
          if (removed.tellp() != 0) {
            removed << ", ";
          }
          removed << num;
        }

        std::stringstream added;
        for (const auto& num : addedNums) {
          if (added.tellp() != 0) {
            added << ", ";
          }
          added << num;
        }

        LOG(INFO) << "Optimization rule " << rule.toString()
                  << " applied.  Replaced tasks [" << removed.str()
                  << "] with [" << added.str() << "]";

        ruleApplied = true;
        break;
      }
    }

  } while (ruleApplied);
}

void Pipeline::defaultSchedule(
    const std::vector<tasks::PipelineTask*>& subtasks) {
  LOG(INFO) << "Using default scheduling";
  // Schedule all Async tasks
  for (auto& task : subtasks) {
    auto num = task->getNum();
    switch (task->getAsyncType()) {
      // Pipelinebraker will be executed in next step
      case tasks::AsyncType::Synchronous:
        break;
      // Schedule async tasks
      case tasks::AsyncType::SingleThreaded:
        LOG(INFO) << "Scheduling task number " << num << " of type "
                  << task->getName();
          threadPool->scheduleFuture(task, 1);
        break;
      case tasks::AsyncType::MultiThreaded:
        LOG(INFO) << "Scheduling task number " << num << " of type "
                  << task->getName();
        threadPool->scheduleFuture(task, 0);
        break;
      default:
        LOG(WARNING) << "Unknown async type for task " << num;
    }
  }

  // Execute all Synchronous tasks
  for (auto& task : subtasks) {
    auto num = task->getNum();
    if (task->getAsyncType() == tasks::AsyncType::Synchronous) {
      threadPool->reschedule();
      LOG(INFO) << "Executing task number " << num << " of type "
                << task->getName();

      auto task_clone = task->clone();
      Timer t;
      size_t iter = 0;
      while (task_clone->execute([](tasks::TaskStatus) { return; }) !=
             tasks::TaskStatus::FINISHED) {
        // Execute until finished
        iter++;
        if(iter > 10){
          threadPool->reschedule();
          iter = 0;
        }
      }
      t.stop();
      taskTimers[num] = std::move(t);
      taskCount[num] = 1;
    }
  }
}


const std::list<tasks::PipelineTaskPtr>& Pipeline::getTasks() const {
  return tasks;
}

}  // namespace joda::queryexecution::pipeline