//
// Created by Nico on 28/01/2019.
//

#ifndef JODA_IOTHREADPOOL_H
#define JODA_IOTHREADPOOL_H

#include <algorithm>
#include <future>
#include <vector>

#include "IThreadUser.h"

/**
 * A templatable threadpool with input and output queues defined by the Worker
 */
template <class Worker>
class IOThreadPool : public IThreadUser {
 public:
  /*
   * Types
   */
  typedef typename Worker::IQueue IQueue;
  typedef typename Worker::OQueue OQueue;
  typedef typename Worker::IPayload IPayload;
  typedef typename Worker::OPayload OPayload;
  typedef typename Worker::WConf WConf;

  /*
   * Constructors
   */
  virtual ~IOThreadPool() = default;

  /**
   * Creates a new IOThreadPool with given IO queues and configuration.
   * The number of threads will be the recommended number of threads for the
   * system.
   * @param iqueue Input queue
   * @param oqueue Output queue
   * @param conf Thread config class
   */
  explicit IOThreadPool(IQueue* iqueue, OQueue* oqueue, WConf conf)
      : IThreadUser(recommendedThreads()),
        iqueue(iqueue),
        oqueue(oqueue),
        workerConfig(conf) {
    while (threads.size() < getMaxThreads()) {
      addThread();
      if (!threads.back()->isRunning()) break;
    }
  }

  /**
   * Creates a new IOThreadPool with given IO queues and configuration.
   * The number of threads will be as specified by the user
   * @param iqueue Input queue
   * @param oqueue Output queue
   * @param maxThreads Maximum number of threads to use in the pool
   * @param conf Thread config class
   */
  explicit IOThreadPool(IQueue* iqueue, OQueue* oqueue, size_t maxThreads,
                        WConf conf)
      : IThreadUser(maxThreads),
        iqueue(iqueue),
        oqueue(oqueue),
        workerConfig(conf) {
    while (threads.size() < getMaxThreads()) {
      addThread();
      if (!threads.back()->isRunning()) break;
    }
  }

  /**
   * Forces the IOThreadPool to use the specified number of threads
   * @param threads number of threads to use
   */
  void forceThreads(size_t threads) override {
    while (threads < getUsedThreads()) {
      addThread();
    }
    while (threads > getUsedThreads()) {
      removeThread();
    }
  }

    /**
   * Increases the number of threads to the given value
   * @param threads number of threads to use
   */
  void increaseThreadsTo(size_t threads) {
    while (threads < getUsedThreads()) {
      addThread();
    }
  }

  /**
   * Returns the number of threads currently in use
   * @return
   */
  size_t getUsedThreads() const override {
    size_t count = 0;
    for (const auto& thread : threads) {
      if (!thread->requestedToStop()) {
        count++;
      }
    }
    return count;
  }

  /**
   * Waits (blocking) for all threads to stop working
   */
  void wait() {
    for (auto& thread : threads) {
      thread->waitFor();
    }
    clearThreads();
  }

  /**
   * Returns the maximum number of threads the IOThreadPool will use
   * @return
   */
  size_t getMaxThreads() const override { return IThreadUser::getMaxThreads(); }

  /**
   * Sets the maximum number of threads the IOThreadPool will use
   * @param maxThreads maximum number of threads
   */
  void setMaxThreads(size_t maxThreads) override {
    IThreadUser::setMaxThreads(maxThreads);
  }

  /**
   * Gets the recommended number of threads for the IOThreadPool template
   * instance
   */
  size_t recommendedThreads() const override {
    return Worker::recommendedThreads();
  }

  /**
   * Checks if there are still threads running
   * @return true if yes, false else
   */
  bool hasRunningThreads() {
    for (const auto& item : threads) {
      if (item->isRunning()) return true;
    }
    return false;
  }

  void setWorkerConfig(const WConf& workerConfig) {
    IOThreadPool::workerConfig = workerConfig;
  }

 protected:
 private:
  std::vector<std::unique_ptr<Worker>> threads{};
  IQueue* iqueue;
  OQueue* oqueue;
  WConf workerConfig;

  void clearThreads() {
    threads.erase(std::remove_if(threads.begin(), threads.end(),
                                 [](const std::unique_ptr<Worker>& o) {
                                   return !o->isRunning();
                                 }),
                  threads.end());
  }
  void removeThread() {
    clearThreads();
    this->threads.back()->stop();
  }

  void addThread() {
    clearThreads();
    this->threads.emplace_back(
        std::make_unique<Worker>(iqueue, oqueue, workerConfig));
    this->threads.back()->start();
  }
};

/*
 * Thread
 */

/**
 * Interface for workers that can be used with the IOThreadPool
 */
template <class IQueueStruct, class OQueueStruct, class WConfig>
class IWorkerThread {
 public:
  /*
   * Types
   */

  typedef typename IQueueStruct::queue_t IQueue;
  typedef typename OQueueStruct::queue_t OQueue;
  typedef typename IQueueStruct::payload_t IPayload;
  typedef typename OQueueStruct::payload_t OPayload;
  typedef WConfig WConf;

  static constexpr auto getIQueueFlags() { return IQueueStruct::getFlag(); };

  static constexpr auto getOQueueFlags() { return OQueueStruct::getFlag(); };

  IWorkerThread(IQueue* iqueue, OQueue* oqueue, const WConf& conf)
      : iqueue(iqueue), oqueue(oqueue), conf(conf){};
  IWorkerThread(IQueue* iqueue, OQueue* oqueue, WConf&& conf)
      : iqueue(iqueue), oqueue(oqueue), conf(std::move(conf)){};
  IWorkerThread(IWorkerThread&&) = default;
  IWorkerThread& operator=(IWorkerThread&&) = default;
  IWorkerThread(const IWorkerThread&) = delete;
  IWorkerThread& operator=(const IWorkerThread&) = delete;
  virtual ~IWorkerThread() { fut.wait(); };

  void start() {
    fut = std::async(std::launch::async, [this] { return this->run(); });
  }
  void waitFor() { fut.wait(); };
  void stop() { shouldRun = false; };
  bool requestedToStop() const { return shouldRun; };
  bool isRunning() const { return running; };
  static const size_t recommendedThreads() { return 1; }

 protected:
  IQueue* iqueue;
  OQueue* oqueue;
  WConf conf;
  bool shouldRun = true;
  virtual void work() = 0;

 private:
  void run() {
    this->work();
    running = false;
  };

  std::future<void> fut;

  bool running = true;
};

#endif  // JODA_IOTHREADPOOL_H
