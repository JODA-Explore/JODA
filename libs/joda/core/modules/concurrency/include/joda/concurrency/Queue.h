//
// Created by Nico on 22/02/2019.
//

#ifndef JODA_QUEUE_H
#define JODA_QUEUE_H
#include <cstdint>
#include <joda/concurrency/concurrentqueue.h>
#include <glog/logging.h>

#define JODA_FLAG_T uint32_t
////////////////////////////////////////////////////////////////////////////////////////////////////
//                Basic Queue
////////////////////////////////////////////////////////////////////////////////////////////////////

#define JODA_QUEUE_WAIT_TIME 1ms
template <class payload_t,JODA_FLAG_T flag=0, class queue_trait = moodycamel::ConcurrentQueueDefaultTraits>
struct JODA_SHARED_QUEUE {
  typedef moodycamel::ConcurrentQueue<payload_t,queue_trait> queue_t;
  typedef typename queue_t::producer_token_t ptok_t;
  typedef typename queue_t::consumer_token_t ctok_t;

  JODA_SHARED_QUEUE(size_t minCapacity, size_t maxExplicitProducers, size_t maxImplicit = 0):queue(minCapacity,maxExplicitProducers,maxImplicit){

  }
  JODA_SHARED_QUEUE():queue(){

  }

  JODA_SHARED_QUEUE(JODA_SHARED_QUEUE& c) = delete;
  JODA_SHARED_QUEUE(JODA_SHARED_QUEUE&& c) = delete;

  constexpr auto getFlag() { return flag; };


  queue_t queue;
  std::atomic<unsigned long> inQueue{};
  std::atomic<unsigned long> registered{};
  std::atomic<unsigned long> finished{};
  std::atomic<size_t> added{};
  std::atomic<size_t> removed{};
  std::atomic<bool> finishedWriting{};

  bool isFinished(){return finishedWriting.load(std::memory_order_acquire) && inQueue.load(std::memory_order_acquire) == 0;}


  std::pair<size_t ,size_t > getStatistics() {
    std::pair<size_t ,size_t > ret = {added.load(std::memory_order_acquire),removed.load(std::memory_order_acquire)};
    return ret;
  };

  void registerProducer(){
    auto i = registered.fetch_add(1);
    DLOG(INFO) << "Registered " << i + 1 << "th producer in queue " << flag;
    DCHECK(i >= 0);
  }

  void unregisterProducer(){
    auto i = registered.fetch_sub(1);
    DLOG(INFO) << "Unregistered " << i << "th producer in queue " << flag;
    DCHECK(i >= 0);
  }

  void producerFinished(){
    auto i = finished.fetch_add(1,std::memory_order_release);
    DLOG(INFO) << i + 1 << " Producer finished in queue " << flag;
    if(finished.load(std::memory_order_acquire) >= registered.load()){
      finishedWriting.store(true,std::memory_order_release);
      DLOG(INFO) << "Queue " << flag << "  finished";
    }
  }


  void send(typename queue_t::producer_token_t& ptok,payload_t&& e){
    using namespace std::chrono_literals;
    auto te = std::move(e);
    while (!queue.try_enqueue(ptok,std::move(te))) {
      std::this_thread::sleep_for(JODA_QUEUE_WAIT_TIME);
    }
    inQueue.fetch_add(1,std::memory_order_release);
    added.fetch_add(1,std::memory_order_relaxed);
  }

  void send(payload_t&& e){
    using namespace std::chrono_literals;
    auto te = std::move(e);
    while (!queue.try_enqueue(std::move(te))) {
      std::this_thread::sleep_for(JODA_QUEUE_WAIT_TIME);
    }
    inQueue.fetch_add(1,std::memory_order_release);
    added.fetch_add(1,std::memory_order_relaxed);
  }

  template<typename It>
  void send(typename queue_t::producer_token_t& ptok,It e,size_t count){
    using namespace std::chrono_literals;
    while(!queue.try_enqueue_bulk(ptok,e,count)){
      std::this_thread::sleep_for(JODA_QUEUE_WAIT_TIME);
    }
    inQueue.fetch_add(count,std::memory_order_release);
    added.fetch_add(count,std::memory_order_relaxed);
  }

  template<typename It>
  void send(It e,size_t count){
    using namespace std::chrono_literals;
    while(!queue.try_enqueue_bulk(e,count)){
      std::this_thread::sleep_for(JODA_QUEUE_WAIT_TIME);
    }
    inQueue.fetch_add(count,std::memory_order_release);
    added.fetch_add(count,std::memory_order_relaxed);
  }


  void retrieve(payload_t& e){
    using namespace std::chrono_literals;
    while(!queue.try_dequeue(e)){
      std::this_thread::sleep_for(JODA_QUEUE_WAIT_TIME);
      if(isFinished()) return;
    }
    inQueue.fetch_sub(1,std::memory_order_release);
    removed.fetch_add(1,std::memory_order_relaxed);
  }

  void retrieve(typename queue_t::consumer_token_t& ctok,payload_t& e){
    using namespace std::chrono_literals;
    while(!queue.try_dequeue(ctok,e)){
      std::this_thread::sleep_for(JODA_QUEUE_WAIT_TIME);
      if(isFinished()) return;
    }
    inQueue.fetch_sub(1,std::memory_order_release);
    removed.fetch_add(1,std::memory_order_relaxed);
  }

  template<typename It>
  size_t retrieve(It e,size_t count){
    auto i = queue.try_dequeue_bulk(e,count);
    if(i > 0) {
      inQueue.fetch_sub(i,std::memory_order_release);
      removed.fetch_add(i,std::memory_order_relaxed);
    }
    return i;
  }
  template<typename It>
  size_t retrieve(typename queue_t::consumer_token_t& ctok,It e,size_t count){
    auto i = queue.try_dequeue_bulk(ctok,e,count);
    if(i > 0) {
      inQueue.fetch_sub(i,std::memory_order_release);
      removed.fetch_add(i,std::memory_order_relaxed);
    }
    return i;
  }
};



struct JODA_NULL_QUEUE {
  typedef bool payload_t;
  typedef JODA_SHARED_QUEUE<payload_t,(JODA_FLAG_T)0> queue_t;


  static std::unique_ptr<queue_t> getQueue(){
    return std::make_unique<queue_t>();
  }
  static std::unique_ptr<queue_t> getQueue(size_t minCapacity, size_t maxProducers) {
    return std::make_unique<queue_t>(minCapacity,maxProducers);
  }
};

typedef JODA_NULL_QUEUE NullQueue;


#endif //JODA_QUEUE_H
