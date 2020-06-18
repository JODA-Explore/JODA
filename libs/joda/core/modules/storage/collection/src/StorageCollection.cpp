//
// Created by Nico Schäfer on 11/16/17.
//

#include "../include/joda/storage/collection/StorageCollection.h"

#include <glog/logging.h>
#include <joda/config/config.h>
#include <joda/join/FileJoinManager.h>
#include <joda/join/JoinManager.h>
#include <joda/join/MemoryJoinManager.h>
#include <joda/misc/MemoryUtility.h>
#include <joda/query/values/IValueProvider.h>

#include <numeric>
#include <regex>

std::shared_ptr<JSONStorage> StorageCollection::getStorage(
    const std::string &name) {
  std::lock_guard<std::mutex> guard(mut);
  if (storages.find(name) == storages.end()) return nullptr;
  return storages[name];
}

void StorageCollection::removeStorage(const std::string &name) {
  std::lock_guard<std::mutex> guard(mut);
  storages.erase(name);
}
std::shared_ptr<JSONStorage> StorageCollection::getOrAddStorage(
    const std::string &name) {
  std::lock_guard<std::mutex> guard(mut);
  if (storages.find(name) == storages.end())
    storages[name] = std::make_shared<JSONStorage>(name);
  return storages[name];
}
std::vector<std::shared_ptr<JSONStorage>> StorageCollection::getStorages() {
  std::lock_guard<std::mutex> guard(mut);
  std::vector<std::shared_ptr<JSONStorage>> ret;
  for (auto &&storage : storages) {
    ret.push_back(storage.second);
  }
  return ret;
}
unsigned long StorageCollection::addTemporaryStorage(
    const std::shared_ptr<JSONStorage> &storage) {
  std::lock_guard<std::mutex> guard(mut);
  auto id = tmpRes.fetch_add(1);
  resultStorage[id] = storage;
  return id;
}
std::shared_ptr<JSONStorage> StorageCollection::getStorage(unsigned long id) {
  std::lock_guard<std::mutex> guard(mut);
  auto tmp = resultStorage.find(id);
  if (tmp == resultStorage.end()) return nullptr;
  return tmp->second;
}
void StorageCollection::removeStorage(unsigned long id) {
  std::lock_guard<std::mutex> guard(mut);
  resultStorage.erase(id);
}
unsigned long StorageCollection::addTemporaryStorage() {
  return addTemporaryStorage(
      std::make_shared<JSONStorage>(JODA_TEMPORARY_STORAGE_NAME));
}
bool StorageCollection::storageExists(const std::string &name) {
  std::lock_guard<std::mutex> guard(mut);
  return storages.find(name) != storages.end();
}

std::vector<std::shared_ptr<JSONStorage>>
StorageCollection::getTemporaryStorages() {
  std::lock_guard<std::mutex> guard(mut);
  std::vector<std::shared_ptr<JSONStorage>> ret;
  for (auto &&storage : resultStorage) {
    ret.push_back(storage.second);
  }
  return ret;
}

std::vector<std::pair<unsigned long, std::shared_ptr<JSONStorage>>>
StorageCollection::getTemporaryIDStorages() {
  std::lock_guard<std::mutex> guard(mut);
  std::vector<std::pair<unsigned long, std::shared_ptr<JSONStorage>>> ret;
  for (auto &&storage : resultStorage) {
    ret.emplace_back(storage.first, storage.second);
  }
  return ret;
}

std::shared_ptr<JoinManager> StorageCollection::getOrStartJoin(
    std::unique_ptr<joda::query::IValueProvider> &&valProv) {
  auto tmpValProv = std::move(valProv);
  if (tmpValProv == nullptr) {
    return nullptr;
  }
  auto jName = getJoinName(*tmpValProv);  // Get Name
  std::lock_guard<std::mutex> guard(mut);
  auto f = runningJoins.find(jName);
  if (f != runningJoins.end()) return f->second;  // If already exist return
  // Else create, store and return
  std::shared_ptr<JoinManager> jm;
  if (!config::storeJson) {
    jm = std::make_shared<FileJoinManager>(jName, std::move(tmpValProv));
    runningJoins[jName] = jm;
  } else {
    jm = std::make_shared<MemoryJoinManager>(jName, std::move(tmpValProv));
    runningJoins[jName] = jm;
  }

  return jm;
}

void StorageCollection::stopJoin(const joda::query::IValueProvider &valProv) {
  auto jName = getJoinName(valProv);  // Get Name
  std::lock_guard<std::mutex> guard(mut);
  runningJoins.erase(jName);  // Remove
}

std::string StorageCollection::getJoinName(const joda::query::IValueProvider &valProv) {
  auto string = valProv.toString();
  auto regex = std::regex(R"([^A-Za-z0-9.-])");
  std::string replaced;
  std::regex_replace(std::back_inserter(replaced), string.begin(), string.end(),
                     regex, "_");
  return replaced;
}

void StorageCollection::stopJoin(const JoinManager &jm) {
  std::lock_guard<std::mutex> guard(mut);
  runningJoins.erase(jm.getName());
}

StorageCollection::~StorageCollection() {
  DLOG(INFO) << "Cleaning up " << storages.size() << " Storages "
             << "(" << resultStorage.size() << " temporaries)";
}

void StorageCollection::ensureSpace(
    long long estimatedSize, const std::shared_ptr<JSONStorage> &except) {
  if (config::evictionStrategy == config::NO_EVICTION) return;
  DCHECK(estimatedSize <
         config::maxmemory);  // TODO: gracefully capture (move no nostore mode)
  while (estimatedSize >= config::maxmemory - estimatedStorageSize()) {
    auto estimatedTyped = MemoryUtility::MemorySize(estimatedSize);
    auto remainingTyped =
        MemoryUtility::MemorySize(config::maxmemory - estimatedStorageSize());
    DLOG(INFO) << "Estimated required size: "
               << estimatedTyped.getHumanReadable()
               << " Remaining size: " << remainingTyped.getHumanReadable();
    auto storage = chooseStorageToRemove(except);
    if (storage == nullptr) {
      LOG(ERROR) << "Could not ensure space, everything already freed";
      return;
    }
    LOG(INFO) << "Freeing storage: " << storage->getName() << " Expected gain: "
              << MemoryUtility::MemorySize(storage->estimatedSize())
                     .getHumanReadable();
    storage->freeAllMemory();
  }
}

std::shared_ptr<JSONStorage> StorageCollection::chooseStorageToRemove(
    const std::shared_ptr<JSONStorage> &except) const {
  std::vector<std::pair<std::shared_ptr<JSONStorage>, size_t>> candidates;
  for (const auto &storage : storages) {
    auto estimation = storage.second->estimatedSize();
    if (storage.second != except &&
        storage.second->estimatedCapacity() >
            0) {  // Not excepted, and not freed or empty
      candidates.emplace_back(storage.second, estimation);
    }
  }
  switch (config::evictionStrategy) {
    case config::LARGEST:
      return chooseLargestStorage(candidates, except);
    case config::LRU:
      return chooseLRUStorage(candidates, except);
    default:
      DCHECK(false) << "Not implemented";
  }

  return nullptr;
}

long long StorageCollection::estimatedStorageSize() const {
  return std::accumulate(storages.begin(), storages.end(), 0l,
                         [](long est, const auto &source) {
                           return est + source.second->estimatedSize();
                         });
}

std::shared_ptr<JSONStorage> StorageCollection::chooseLargestStorage(
    std::vector<std::pair<std::shared_ptr<JSONStorage>, size_t>> &candidates,
    const std::shared_ptr<JSONStorage> &except) const {
  std::sort(candidates.begin(), candidates.end(),
            [](const std::pair<std::shared_ptr<JSONStorage>, size_t> &a,
               const std::pair<std::shared_ptr<JSONStorage>, size_t> &b) {
              return (a.second > b.second);
            });
  if (candidates.empty()) return nullptr;
  return candidates.front().first;
}

std::shared_ptr<JSONStorage> StorageCollection::chooseLRUStorage(
    std::vector<std::pair<std::shared_ptr<JSONStorage>, size_t>> &candidates,
    const std::shared_ptr<JSONStorage> &except) const {
  std::sort(candidates.begin(), candidates.end(),
            [](const std::pair<std::shared_ptr<JSONStorage>, size_t> &a,
               const std::pair<std::shared_ptr<JSONStorage>, size_t> &b) {
              return (a.first->getLastUsed() < b.first->getLastUsed());
            });
  if (candidates.empty()) return nullptr;
  return candidates.front().first;
}
