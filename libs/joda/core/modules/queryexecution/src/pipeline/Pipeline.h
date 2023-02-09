#ifndef JODA_PIPELINE_H
#define JODA_PIPELINE_H

#include <list>
#include <joda/pipelineatomics/PipelineTask.h>
#include <joda/misc/Timer.h>
#include "ThreadPool.h"
#include "optimization/OptimizationRule.h"

namespace joda::queryexecution::pipeline {

  /**
   * @brief Pipelines are responsible for execution of arbitrary ordered code in JODA
   * 
   */
  class Pipeline {
   private:
   using TaskList = std::list<tasks::PipelineTaskPtr>;

   public:
    Pipeline(std::shared_ptr<ThreadPool>& threadPool) : currNum(tasks::PipelineTask::UNSET_NUM), threadPool(threadPool) {}

    ~Pipeline() = default;
    /**
     * @brief Fully executes the pipeline with all synchronous and asynchronous tasks.
     * 
     */
    void execute();

    /**
     * @brief Adds a task to the pipeline
     * 
     * @param task The task to add
     */
    void addTask(tasks::PipelineTaskPtr&& task);

    /**
     * @brief Checks if there are still tasks left in the pipeline
     * @return true if tasks remain
     * @return false otherwise
     */
    bool hasTasks() const;

    /**
     * @brief Returns the number of tasks remaining in the pipeline
     * 
     * @return size_t Number of tasks in the pipeline
     */
    size_t numTasks() const;

    /**
     * @brief Finalizes the pipeline by filling missing data and optimizing tasks.
     * Finalize HAS to be called after adding all tasks and before executing the pipeline
     * 
     */
    void finalize();

    /**
     * @brief Optimizes the pipeline with the given rules
     */
    void optimize(std::vector<std::unique_ptr<optimization::OptimizationRule>>& optimizations);

    /**
     * @brief Returns the pipeline in JSON representation
     * The document has the following structure:
     * {
     *    "MaxThreads" : 8,
     *    "Tasks": [
     *       {"Num": 1, "ID": 4, "Name": "ListFiles", "Async": "Synchronous"},
     *       ...
     *     ],
     *    "Connections": [
     *      {"From": [1], "To": [2], "Throughput": 100},
     *    ]
     * }
     * @return RJDocument The json representing the pipeline
     */
    RJDocument toJSON() const;

    /**
     * @brief Returns the names of the pipeline tasks
     * @return std::vector<std::string> The names of the pipeline tasks
     */
    std::vector<std::string> getTaskNameList() const;

    const TaskList& getTasks() const;


    private:
      // List of all tasks
      TaskList tasks;
      // Current sequential task id
      tasks::TaskNum currNum;
      // Threadpool
      std::shared_ptr<ThreadPool> threadPool;
      // Input Queues
      std::unordered_map<tasks::TaskNum, pipeline::PipelineQueuePtr> queues;
      // Outputs
      std::vector<std::pair<tasks::TaskNum,tasks::TaskNum>> connections;

      // --- Stats ---
      // Task timers
      std::unordered_map<tasks::TaskNum, Timer> taskTimers;
      std::unordered_map<tasks::TaskNum, tasks::TaskStatistics> taskStatistics;
      // Number of tasks
      std::unordered_map<tasks::TaskNum, size_t> taskCount;
      


      tasks::TaskNum addTaskAt(tasks::PipelineTaskPtr&& task,TaskList::iterator& at);

      std::vector<std::vector<tasks::PipelineTask*>> splitIndependentPipelines() const;

      // --- Scheduling strategies ---
      /**
       * Schedules all tasks until the first pipeline breaker and then adds the rest of the tasks into the queue
       */
      void defaultSchedule(const std::vector<tasks::PipelineTask*>& tasks);

      


  };


} // namespace joda::queryexecution::pipeline
#endif // JODA_PIPELINE_H