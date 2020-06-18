//
// Created by Nico on 06/12/2018.
//

#ifndef JODA_THREADMANAGER_H
#define JODA_THREADMANAGER_H

#include <glob.h>
#include <memory>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <thread>
#include <glog/logging.h>
#include "IThreadUser.h"

#define JODA_THREAD_POLLING_TIME 500ms

class ThreadManager {
 public:
  ThreadManager(size_t maxThreads);
  ThreadManager();
  size_t getMaxThreads() const;
  void setMaxThreads(size_t maxThreads);
  size_t registerThreadUser(IThreadUser *user);
  void unregisterThreadUser(size_t id);
  void unregisterThreadUser(std::vector<size_t> &ids);
  template <class queue_t>
  void balanceQueue(std::vector<size_t>& writerIds,std::vector<size_t>& readerIds,queue_t& queue,bool& stop){
    std::pair<size_t ,size_t > prevStats;
      while(!stop){
        std::pair<size_t ,size_t > stats = queue->getStatistics();
        std::pair<size_t ,size_t > currStats = stats;
        currStats.first -= prevStats.first;
        currStats.second -= prevStats.second;
        prevStats = stats;
        size_t i = 0;
        for (const auto &id : writerIds) {
          i += users[id]->getUsedThreads();
        }
        size_t o = 0;
        for (const auto &id : readerIds) {
          o += users[id]->getUsedThreads();
        }
        DLOG(INFO) << "QUEUE I/O: " << currStats.first<<"/"<<currStats.second<<" | "<< "Threads I/O: " << i<<"/"<<o;
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(JODA_THREAD_POLLING_TIME);
      }

  }
 private:

  std::mutex mut;
  void recalculateThreads();
  size_t maxThreads = 1;
  size_t usedThreads() const;
  size_t reservedThreads() const;
  size_t availableThreads() const;
  std::unordered_map<size_t, IThreadUser *> users;
  size_t currID = 0;

};

extern ThreadManager g_ThreadManagerInstance;

#endif //JODA_THREADMANAGER_H
