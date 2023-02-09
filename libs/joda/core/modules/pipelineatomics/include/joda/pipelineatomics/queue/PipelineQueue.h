#ifndef JODA_PIPELINE_QUEUE_H
#define JODA_PIPELINE_QUEUE_H

#include <memory>

namespace joda::queryexecution::pipeline {
  typedef size_t QueueNum;
  typedef std::nullptr_t NoOpt;

  /**
   * @brief Simple interface for a single pipeline Queue
   * 
   */
  class PipelineQueue {
   public:
    virtual ~PipelineQueue() = default;
    /**
     * @brief Wether or not the queue is completed and can be destroyed
     * 
     * @return true If the queue is finished 
     * @return false otherwise
     */
    virtual bool isFinished() = 0;

    /**
     * @brief Returns the number of elements submitted through the queue
     * 
     * @return size_t #Elements submitted though the queue
     */
    virtual size_t getThroughput() = 0;

    /**
     * @brief Returns the Num of the queue
     * 
     * @return QueueNum Num of the queue
     */
    virtual QueueNum getNum() const = 0;
  };

  typedef std::unique_ptr<PipelineQueue> PipelineQueuePtr;
} // namespace joda::queryexecution::pipeline
#endif // JODA_PIPELINE_QUEUE_H