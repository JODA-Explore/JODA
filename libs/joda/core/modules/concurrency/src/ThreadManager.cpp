//
// Created by Nico on 06/12/2018.
//

#include "../include/joda/concurrency/ThreadManager.h"
#include <cmath>

ThreadManager g_ThreadManagerInstance;

size_t ThreadManager::getMaxThreads() const { return maxThreads; }

void ThreadManager::setMaxThreads(size_t maxThreads) {
  std::lock_guard<std::mutex> guard(mut);
  DCHECK(maxThreads > 0) << "Threads should always be greater 0";
  if (ThreadManager::maxThreads != maxThreads) {
    ThreadManager::maxThreads = maxThreads;
    recalculateThreads();
  }
}

size_t ThreadManager::registerThreadUser(IThreadUser* user) {
  std::lock_guard<std::mutex> guard(mut);
  auto i = ++currID;
  users[i] = user;
  DLOG(INFO) << "Registered Thread #" << i;
  recalculateThreads();
  return i;
}

void ThreadManager::unregisterThreadUser(size_t id) {
  std::lock_guard<std::mutex> guard(mut);
  DCHECK(id > 0 && id <= currID);
  users.erase(id);
  DLOG(INFO) << "Unregistered Thread #" << id;
  recalculateThreads();
}

void ThreadManager::unregisterThreadUser(std::vector<size_t>& ids) {
  std::lock_guard<std::mutex> guard(mut);
  for (const auto& id : ids) {
    DCHECK(id > 0 && id <= currID);
    users.erase(id);
    DLOG(INFO) << "Unregistered Thread #" << id;
  }
  recalculateThreads();
}

size_t ThreadManager::usedThreads() const {
  size_t threads = 0;
  for (const auto& user : users) {
    threads += user.second->getUsedThreads();
  }
  return threads;
}

size_t ThreadManager::reservedThreads() const {
  size_t threads = 0;
  for (const auto& user : users) {
    threads += user.second->getMaxThreads();
  }
  return threads;
}

size_t ThreadManager::availableThreads() const {
  return maxThreads - reservedThreads();
}

ThreadManager::ThreadManager(size_t maxThreads) : maxThreads(maxThreads) {
  DCHECK(maxThreads > 0) << "Threads should always be greater 0";
}

void ThreadManager::recalculateThreads(){
#define JODA_TM_PTR_INDEX 0
#define JODA_TM_ID_INDEX 1
#define JODA_TM_T_INDEX 2
    /*
  if (users.empty()) return;
  auto inUse = usedThreads();
  auto reserved = reservedThreads();
  //Pointer, ID, Threads
  std::vector<std::tuple<IThreadUser *, size_t, size_t>> threads;
  threads.reserve(users.size());
  if (reserved == maxThreads) return; //Nothing to do

  //Collect requested values
  size_t numMaxThreads = 0;
  size_t requestedThreads = 0;
  for (const auto &user : users) {
    threads.emplace_back(user.second, user.first,
  user.second->recommendedThreads()); auto t =
  std::get<JODA_TM_T_INDEX>(threads.back()); if (t < JODA_THREAD_MAX)
  requestedThreads += t; else numMaxThreads++;
  }
  if (requestedThreads + numMaxThreads <= maxThreads) { //We have enough threads
    if (numMaxThreads > 0) { //Are there even Thread users that have to be
  distributed? auto remainingThreads = maxThreads - requestedThreads;
      DCHECK(remainingThreads >= numMaxThreads);
      auto distributedMaxThreads = remainingThreads / numMaxThreads;
      for (auto &item : threads) {
        if (std::get<JODA_TM_T_INDEX>(item) == JODA_THREAD_MAX)
  std::get<JODA_TM_T_INDEX>(item) = distributedMaxThreads;
      }
    }
  } else { //We do not have enough threads
    if (threads.size() >= maxThreads) { //Special case: More jobs than threads
  => everyone gets exactly 1 for (auto &item : threads) {
        std::get<JODA_TM_T_INDEX>(item) = 1;
      }
    } else {
      auto factor = (static_cast<double>(maxThreads)) / (requestedThreads +
  numMaxThreads); for (auto &item : threads) {
        //All specific recommended
        if (std::get<JODA_TM_T_INDEX>(item) < JODA_THREAD_MAX)
          std::get<JODA_TM_T_INDEX>(item) = std::max(size_t(1),
                                                     static_cast<size_t>(std::trunc(
                                                         factor *
  std::get<JODA_TM_T_INDEX>(item)))); else std::get<JODA_TM_T_INDEX>(item) = 1;
      }
    }
  }

  //Debug
  std::string debug;
  size_t allthreads = 0;
  for (const auto &t : threads) {
    debug += std::to_string(std::get<JODA_TM_ID_INDEX>(t)) + ":" +
  std::to_string(std::get<JODA_TM_T_INDEX>(t)) + " "; allthreads +=
  std::get<JODA_TM_T_INDEX>(t);
  }


  //Set Threads
  for (const auto &thread : threads) {
    std::get<JODA_TM_PTR_INDEX>(thread)->setMaxThreads(std::get<JODA_TM_T_INDEX>(thread));
    //if (std::get<JODA_TM_PTR_INDEX>(thread)->getUsedThreads() >
  std::get<JODA_TM_T_INDEX>(thread))
     //
  std::get<JODA_TM_PTR_INDEX>(thread)->forceThreads(std::get<JODA_TM_T_INDEX>(thread));
  }


  DLOG(INFO) << "Reshuffled " << allthreads << " threads to " << debug;
      */
}

ThreadManager::ThreadManager() = default;
