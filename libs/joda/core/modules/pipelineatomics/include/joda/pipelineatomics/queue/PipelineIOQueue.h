#ifndef JODA_PIPELINE_IOQUEUE_H
#define JODA_PIPELINE_IOQUEUE_H

#include <memory>
#include <queue>
#include <joda/concurrency/MPMCQueue.h>
#include "PipelineQueue.h"
#include <mutex>

namespace joda::queryexecution::pipeline {

/**
 * @brief A single pipeline Queue
 * 
 * @tparam Element The type of the queue
 */
  template<class Element, size_t size>
  class PipelineIOQueue : public PipelineQueue {
   protected:

   public:
    // The queue to use 
    using Queue_t = rigtorp::MPMCQueue<Element>;


    virtual ~PipelineIOQueue() = default;

    /**
     * @brief Construct a new Pipeline IO Queue 
     * 
     * @param num The num of the queue
     * @param capacity The  capacity of the queue
     */
    PipelineIOQueue(QueueNum num, size_t capacity): _num(num), _queue(capacity) {

    } 

    void registerProducer() {
      producers.fetch_add(1);
    }

    void unregisterProducer() {
      auto prev = producers.fetch_sub(1);
      if(prev == 1){
        LOG(INFO) << "Queue finished: " << _num ;
      }
    }



    /**
     * @brief Returns a reference to the internal queue
     * 
     * @return The queue
     */
    Queue_t& getQueue() {
      return _queue;
    }

    virtual QueueNum getNum() const override {
      return _num;
    }

    virtual bool isFinished() override {
      return producers.load() == 0 &&  _queue.size() == 0;
    }

    virtual size_t getThroughput() override {
      //TODO
      return 0;
    }

    private:
    QueueNum _num;
    Queue_t _queue;
    std::atomic_size_t producers = 0;
    
  };

} // namespace joda::queryexecution::pipeline
#endif // JODA_PIPELINE_IOQUEUE_H