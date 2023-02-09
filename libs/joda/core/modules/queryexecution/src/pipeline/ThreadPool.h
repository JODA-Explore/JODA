#ifndef JODA_THREADPOOL_H
#define JODA_THREADPOOL_H

#include <joda/misc/RecurringTimer.h>
#include <joda/pipelineatomics/PipelineTask.h>

#include <boost/asio.hpp>

#include <cstdio>
#include <future>
#include <map>

namespace joda::queryexecution::pipeline {

class ThreadPool {
 private:
  struct ScheduledTask {
    ScheduledTask(tasks::PipelineTaskPtr&& task) : task(std::move(task)) {}

    tasks::PipelineTaskPtr task;
    tasks::TaskStatus status = tasks::TaskStatus::NOTSTARTED;
  };

 public:
  ThreadPool(size_t maxThreads) : max_threads(maxThreads), pool(maxThreads) {}

  /**
   * @brief Returns the maximum amount of supported threads
   *
   * @return size_t Number of maximum supported threads
   */
  size_t getMaxThreads() const { return max_threads; }

  size_t getAvailableThreads() const { return max_threads - running_threads; }

  /**
   * @brief Schedules a task to be started later
   * No task is started immediately, but it will be started when a rescheduling
   * occurs
   *
   * @param task The task to schedule
   * @param max_instances Maximum amount of threads to schedule. If 0, all
   * threads are used
   */
  void scheduleFuture(tasks::PipelineTask* task, size_t max_instances = 0) {
    if (max_instances == 0) {  // max_instances = 0 => max threads
      max_instances = max_threads;
    }

    if (max_instances > max_threads) {  // Limit to max_threads
      max_instances = max_threads;
    }

    std::vector<std::unique_ptr<ScheduledTask>> stage;
    for (size_t i = 0; i < max_instances; i++) {
      stage.emplace_back(std::make_unique<ScheduledTask>(task->clone()));
    }

    LOG(INFO) << "Scheduled task " << task->getNum()
              << "(Type: " << task->getName() << ") with a maximum of "
              << max_instances << " threads for future starting.";

    std::lock_guard lock(mut);
    stages[task->getNum()] = std::move(stage);
  }

  /**
   * @brief Cleans up finished tasks and threads and reschedules available
   * threads
   *
   */
  void reschedule() { rescheduleTask(nullptr); }

  /**
   * @brief Cleans up finished tasks and threads and reschedules available
   * threads
   *
   */
  void rescheduleTask(ScheduledTask* task,
                      tasks::TaskStatus status = tasks::TaskStatus::FINISHED) {
    std::lock_guard guard(mut);
    if (task != nullptr) {
      running_threads--;
      task->status = status;

    }


    // Clear finished
    if (task != nullptr && task->status == tasks::TaskStatus::FINISHED) {
      LOG(INFO) << "Task " << task->task->getNum() << "("
                << task->task->getName() << ") finished.";
      auto& stage = stages[task->task->getNum()];
      for (size_t i = 0; i < stage.size(); i++) {
        // Remove finished task
        if (task == stage[i].get()) {
          auto num = task->task->getNum();
          auto name = task->task->getName();
          // TODO get stats
          stage.erase(stage.begin() + i);
          if (stage.empty()) {
            LOG(INFO) << "Stage " << num << "(" << name << ") finished.";
            // Remove finished stage
            stages.erase(num);
          }
          task = nullptr;
          // TODO if task finished & removed reschedule strategy to next stage
          break;
        }
      }
    }

    if (stages.empty()) {
      DLOG(INFO) << "No more tasks to schedule";
      return;
    }

    // Reschedule thread
    auto stage = scheduleStrategy(task);

    // While threads can be scheduled and there are stages
    const auto startStage = stage;
    bool couldStart = false;
    while (running_threads < max_threads && !stages.empty()) {
      // Iterate tasks of stage
      for (auto& task : stage->second) {
        // If not already started => start
        if (task->status != tasks::TaskStatus::FINISHED &&
            task->status != tasks::TaskStatus::RUNNING) {
          task->status = tasks::TaskStatus::RUNNING;
          running_threads++;
          auto* task_ptr = task.get();

          boost::asio::post(pool,[this, task_ptr]() {
            return task_ptr->task->execute(
                std::bind(&ThreadPool::rescheduleTask, this, task_ptr,
                          std::placeholders::_1));
          });

          couldStart = true;
          break;
        }
      }
      stage++;
      if (stage == stages.end()) {
        // Circle around
        stage = stages.begin();
      }

      if (stage == startStage) {
        if (couldStart) {
          couldStart = false;
        } else {
          // If no more tasks can be started, break
          break;
        }
      }
    }
  }

  void wait() {
    while (true) {
      {
        std::lock_guard guard(mut);
        if (stages.begin() == stages.end()) {
          break;
        }
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::lock_guard guard(mut);
    pool.join();

    DCHECK(stages.empty() && running_threads == 0);
  }

  void reset() {
    std::lock_guard guard(mut);
    stages.clear();
    running_threads = 0;
    //Reset Threadpool
    pool.join();
    pool.~thread_pool();
    new (&pool) boost::asio::thread_pool(max_threads);
  }

  void fillStats(
      std::unordered_map<tasks::TaskNum, Timer>& timers,
      std::unordered_map<tasks::TaskNum, tasks::TaskStatistics>& statistics,
      std::unordered_map<tasks::TaskNum, size_t>& threadCounts) {
    std::lock_guard<std::mutex> guard(mut);
    for (auto& task : taskTimers) {
      timers[task.first] = task.second;
    }
    for (auto& task : taskStats) {
      statistics[task.first] = task.second;
    }
    for (auto& task : taskScheduleCount) {
      threadCounts[task.first] = task.second;
    }
  }

 private:
  size_t max_threads;
  size_t running_threads = 0;
  std::mutex mut;


  std::map<tasks::TaskNum, std::vector<std::unique_ptr<ScheduledTask>>> stages;

  // Pool
  boost::asio::thread_pool pool;

  // --- Stats ---
  std::unordered_map<tasks::TaskNum, Timer> taskTimers;
  std::unordered_map<tasks::TaskNum, tasks::TaskStatistics> taskStats;
  std::unordered_map<tasks::TaskNum, size_t> taskScheduleCount;
  std::unordered_map<tasks::TaskNum, size_t> taskCurrCount;


  std::map<tasks::TaskNum, std::vector<std::unique_ptr<ScheduledTask>>>::const_iterator
   scheduleStrategy(ScheduledTask* task){
    // Begin at front if rescheduling is desired
    if(task == nullptr || task->status == tasks::TaskStatus::FINISHED) return stages.begin();

    auto num = task->task->getNum();
    auto status = task->status;

    // Stage of current task
    auto taskStage = stages.find(num);
    if(status == tasks::TaskStatus::STARVED){
      // Starved task, reschedule same stage
    } else if(status == tasks::TaskStatus::YIELD){
      // Yielded task, reschedule to next
      taskStage++;
    } else if(status == tasks::TaskStatus::IDLE){
      // Idle task, reschedule to next
      taskStage++;
    }else{
      // Unknown, reschedule from front
      return stages.begin();
    }
    if(taskStage == stages.end()){
      return stages.begin();
    }
    return taskStage;
  }
};

}  // namespace joda::queryexecution::pipeline

#endif  // JODA_THREADPOOL_H