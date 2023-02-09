#ifndef JODA_PIPELINE_TASK_H
#define JODA_PIPELINE_TASK_H

#include <memory>
#include <functional>
#include <chrono>
#include <joda/misc/RJFwd.h>
#include "queue/PipelineQueue.h"
#include "Async.h"
#include "TaskID.h"

namespace joda::queryexecution::pipeline::tasks {
  using TaskNum = size_t;

  enum class TaskStatus { NOTSTARTED, RUNNING, FINISHED, IDLE, STARVED, YIELD };

  inline const char* ToString(TaskStatus v)
{
    switch (v)
    {
        case TaskStatus::NOTSTARTED:   return "NOT_STARTED";
        case TaskStatus::RUNNING:   return "RUNNING";
        case TaskStatus::FINISHED: return "FINISHED";
        case TaskStatus::IDLE: return "IDLE";
        case TaskStatus::STARVED: return "STARVED";
        case TaskStatus::YIELD: return "YIELD";
        default:      return "UNKNOWN STATUS";
    }
}

  /**
   * @brief Statistics of the task execution
   * 
   */
  struct TaskStatistics {
    std::chrono::milliseconds runtime {};
    std::chrono::milliseconds max_runtime {};
    std::chrono::milliseconds min_runtime {};
    unsigned long input = 0;
    unsigned long output = 0;
    bool ran = false;

    void operator+= (const TaskStatistics & other){
      if(!ran){
        ran = other.ran;
        runtime = other.runtime;
        max_runtime = other.max_runtime;
        min_runtime = other.min_runtime;
        input = other.input;
        output = other.output;
      }
      else if(other.ran){
       runtime += other.runtime;
       if(other.max_runtime > max_runtime) max_runtime = other.max_runtime;
       if(other.min_runtime < min_runtime) min_runtime = other.min_runtime;
       input += other.input;
       output += other.output;
      }
    }
  };

  /**
   * @brief Simple interface for a single executable pipeline Task
   * 
   */
  class PipelineTask {
   public:
    static constexpr TaskNum UNSET_NUM = 0;

    virtual ~PipelineTask() = default;
    /**
     * @brief Executes the pipeline task
     * 
     */
    virtual TaskStatus execute(std::function<void(TaskStatus)> callback) = 0;

    /**
     * @brief Returns the (unique) name of the task
     * 
     * @return name of the task
     */
    virtual TaskID getName() const = 0;

    /**
     * @brief Returns the Num of the task
     * 
     * @return TaskNum Num of the task
     */
    virtual TaskNum getNum() const = 0;

    /**
     * @brief Sets the Num of the task
     * 
     * @param id the new Num of the task
     */
    virtual void setNum(TaskNum id) = 0;

    /**
     * @brief Get the async type of the task
     * 
     * @return AsyncType the type of the task
     */
    virtual AsyncType getAsyncType() const = 0;

    /**
     * @brief Wether or not the input queue is connected (if required at all)
     * 
     * @return true If the input queue is connected, or no input is required
     * @return false Otherwise
     */
    virtual bool inputConnected() const = 0;

        /**
     * @brief Wether or not the output queue is connected (if required at all)
     * 
     * @return true If the output queue is connected, or no output is required
     * @return false Otherwise
     */
    virtual bool outputConnected() const = 0;

    /**
     * @brief Creates and sets an input queue for the given task
     * 
     * @return PipelineQueuePtr A unique pointer to the created input queue
     */
    virtual PipelineQueuePtr createInputQueue(size_t max_threads)  = 0;

    /**
     * @brief swaps the input queue of the task with the given task
     */
    virtual void setInputQueue(PipelineQueue* queue) = 0;


    /**
     * @brief Set the output queue of the task 
     * 
     * @param queue the queue to use
     * @param producer wether or not the task is an actual producer (true), or only a template (false)
     */
    virtual void setOutputQueue(PipelineQueue* queue) = 0;

    /**
     * @brief Wether or not the task is finished
     * 
     * @return true if the task finished completely
     * @return false otherwise
     */
    virtual bool isFinished() const = 0;
    
    /**
     * @brief Clones the given Task
     * 
     * @return PipelineTaskPtr A clone of the task
     */
    virtual std::unique_ptr<PipelineTask> clone() = 0;

    /**
     * @brief Creates a JSON representation of the task
     * 
     * @param alloc A memory pool allocator to use in rapidjson construction
     * @return RJValue A rapidjson value containing the task representation as JSON
     */
    virtual RJValue toJSON(RJMemoryPoolAlloc& alloc) const = 0;

    virtual TaskStatistics getStatistics() const = 0;
  };

  typedef std::unique_ptr<PipelineTask> PipelineTaskPtr;
} // namespace joda::queryexecution::pipeline
#endif // JODA_PIPELINE_TASK_H