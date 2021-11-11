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
#include <algorithm>
#include <numeric>
#include <regex>

std::shared_ptr<JSONStorage> StorageCollection::getStorage(
    const std::string& name) {
  std::lock_guard<std::mutex> guard(mut);
  if (storages.find(name) == storages.end()) {
    return nullptr;
  }
  return storages[name];
}

void StorageCollection::removeStorage(const std::string& name) {
  LOG(INFO) << "Removing storage " << name;
  std::lock_guard<std::mutex> guard(mut);
  storages.erase(name);
  storageInsertion.erase(
      std::remove(storageInsertion.begin(), storageInsertion.end(), name),
      storageInsertion.end());
  bool finished = false;
  while (!finished) {
    auto it = std::find_if(
        dependencies.begin(), dependencies.end(),
        [&name](const std::pair<std::shared_ptr<JSONStorage>,
                                std::shared_ptr<JSONStorage>>& p) {
          return p.first->getName() == name || p.second->getName() == name;
        });
    if (it == dependencies.end()) {
      finished = true;
    } else {
      dependencies.erase(it);
    }
  }
}
std::shared_ptr<JSONStorage> StorageCollection::getOrAddStorage(
    const std::string& name) {
  std::lock_guard<std::mutex> guard(mut);
  if (storages.find(name) == storages.end()) {
    LOG(INFO) << "Adding storage " << name;
    storages[name] = std::make_shared<JSONStorage>(name);
    storageInsertion.emplace_back(name);
  }

  return storages[name];
}
std::vector<std::shared_ptr<JSONStorage>> StorageCollection::getStorages() {
  std::lock_guard<std::mutex> guard(mut);
  std::vector<std::shared_ptr<JSONStorage>> ret;
  for (auto&& storage : storages) {
    ret.push_back(storage.second);
  }
  return ret;
}
unsigned long StorageCollection::addTemporaryStorage(
    const std::shared_ptr<JSONStorage>& storage) {
  std::lock_guard<std::mutex> guard(mut);
  auto id = tmpRes.fetch_add(1);
  resultStorage[id] = storage;
  return id;
}
std::shared_ptr<JSONStorage> StorageCollection::getStorage(unsigned long id) {
  std::lock_guard<std::mutex> guard(mut);
  auto tmp = resultStorage.find(id);
  if (tmp == resultStorage.end()) {
    return nullptr;
  }
  return tmp->second;
}
void StorageCollection::removeStorage(unsigned long id) {
  LOG(INFO) << "Removing result " << id;
  std::lock_guard<std::mutex> guard(mut);
  resultStorage.erase(id);
}
unsigned long StorageCollection::addTemporaryStorage() {
  LOG(INFO) << "Creating temporary storage";
  return addTemporaryStorage(
      std::make_shared<JSONStorage>(JODA_TEMPORARY_STORAGE_NAME));
}
bool StorageCollection::storageExists(const std::string& name) {
  std::lock_guard<std::mutex> guard(mut);
  return storages.find(name) != storages.end();
}

std::vector<std::shared_ptr<JSONStorage>>
StorageCollection::getTemporaryStorages() {
  std::lock_guard<std::mutex> guard(mut);
  std::vector<std::shared_ptr<JSONStorage>> ret;
  for (auto&& storage : resultStorage) {
    ret.push_back(storage.second);
  }
  return ret;
}

std::vector<std::pair<unsigned long, std::shared_ptr<JSONStorage>>>
StorageCollection::getTemporaryIDStorages() {
  std::lock_guard<std::mutex> guard(mut);
  std::vector<std::pair<unsigned long, std::shared_ptr<JSONStorage>>> ret;
  for (auto&& storage : resultStorage) {
    ret.emplace_back(storage.first, storage.second);
  }
  return ret;
}

std::shared_ptr<JoinManager> StorageCollection::getOrStartJoin(
    std::unique_ptr<joda::query::IValueProvider>&& valProv) {
  auto tmpValProv = std::move(valProv);
  if (tmpValProv == nullptr) {
    return nullptr;
  }
  auto jName = getJoinName(*tmpValProv);  // Get Name
  LOG(INFO) << "Starting join " << jName;
  std::lock_guard<std::mutex> guard(mut);
  auto f = runningJoins.find(jName);
  if (f != runningJoins.end()) {
    return f->second;  // If already exist return
  }
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

void StorageCollection::stopJoin(const joda::query::IValueProvider& valProv) {
  auto jName = getJoinName(valProv);  // Get Name
  LOG(INFO) << "Stopping join " << jName;
  std::lock_guard<std::mutex> guard(mut);
  runningJoins.erase(jName);  // Remove
}

std::string StorageCollection::getJoinName(
    const joda::query::IValueProvider& valProv) {
  auto string = valProv.toString();
  auto regex = std::regex(R"([^A-Za-z0-9.-])");
  std::string replaced;
  std::regex_replace(std::back_inserter(replaced), string.begin(), string.end(),
                     regex, "_");
  return replaced;
}

void StorageCollection::stopJoin(const JoinManager& jm) {
  std::lock_guard<std::mutex> guard(mut);
  LOG(INFO) << "Stopping join " << jm.getName();
  runningJoins.erase(jm.getName());
}

StorageCollection::~StorageCollection() {
  DLOG(INFO) << "Cleaning up " << storages.size() << " Storages "
             << "(" << resultStorage.size() << " temporaries)";
  runningJoins.clear();
  dependencies.clear();
  for (auto& storage : storages) {
    storage.second->preparePurge();
  }
  for (auto& storage : resultStorage) {
    storage.second->preparePurge();
  }
  resultStorage.clear();
  while (!storageInsertion.empty()) {
    DLOG(INFO) << "Storage " << storageInsertion.back() << " reference count: "
               << storages[storageInsertion.back()].use_count();
    storages.erase(storageInsertion.back());
    storageInsertion.pop_back();
  }
}

void StorageCollection::ensureSpace(
    unsigned long long estimatedSize,
    const std::shared_ptr<JSONStorage>& withoutDependency) {
  std::lock_guard<std::mutex> guard(mut);
  if (config::evictionStrategy == config::NO_EVICTION) {
    return;
  }
  if (estimatedSize > config::maxmemory) {
    LOG(WARNING)
        << "Estimated required size: "
        << MemoryUtility::MemorySize(estimatedSize).getHumanReadable()
        << " but only "
        << MemoryUtility::MemorySize(config::maxmemory).getHumanReadable()
        << " available to use, freeing as much as possible.";
  }
  auto estimatedCurrentlyStored = estimatedStorageSize();
  auto estimatedHierarchy = estimateHierarchySize(withoutDependency);
  auto estimatedParsedHierarchy = parsedHierarchySize(withoutDependency);
  auto estimatedStorageSize1 = estimatedCurrentlyStored +
                               (estimatedParsedHierarchy - estimatedHierarchy);
  auto toFree = static_cast<double>(config::maxmemory) -
                static_cast<double>(estimatedStorageSize1) -
                static_cast<double>(estimatedSize);
  toFree = -toFree;
  if (toFree > 0) {
    unsigned long long freed = 0;
    long count = 0;
    auto except = getDependencies(withoutDependency);
    auto stores = chooseStoragesToRemove(toFree, except);
    LOG(INFO)
        << "Estimated required size: "
        << MemoryUtility::MemorySize(estimatedSize).getHumanReadable()
        << " Estimated Storage: "
        << MemoryUtility::MemorySize(estimatedStorageSize1).getHumanReadable()
        << " Remaining size: "
        << MemoryUtility::MemorySize(config::maxmemory - estimatedStorageSize1)
               .getHumanReadable()
        << " To Be Freed: "
        << MemoryUtility::MemorySize(toFree).getHumanReadable()
        << " Choosing from " << stores.size() << " collections";
    for (auto& storage : stores) {
      if (static_cast<double>(freed) > toFree) {
        break;
      }
      auto size = storage.second;
      storage.first->freeAllMemory();
      freed += size - storage.first->estimatedSize();
      count++;
      LOG(INFO) << "Freeing storage: " << storage.first->getName()
                << " Expected gain: "
                << MemoryUtility::MemorySize(storage.second).getHumanReadable();
    }
  }
}

std::vector<std::pair<JSONStorage*, size_t>>
StorageCollection::chooseStoragesToRemove(
    long long /*toFree*/, const std::vector<JSONStorage*>& except) const {
  std::vector<std::pair<JSONStorage*, size_t>> candidates;
  int excepted = 0;
  int sizeZero = 0;
  for (const auto& storage : storages) {
    if (std::find(except.begin(), except.end(), storage.second.get()) !=
        except.end()) {
      excepted++;
      continue;
    }
    if (storage.second->estimatedCapacity() <= 0) {
      sizeZero++;
      continue;
    }
    candidates.emplace_back(storage.second.get(),
                            storage.second->estimatedSize());
  }
  LOG(INFO) << "Ignored " << excepted + sizeZero << " of " << storages.size()
            << " collections (" << excepted << " exceptions; " << sizeZero
            << " empty)";
  switch (config::evictionStrategy) {
    case config::LARGEST:
      orderContainerBySize(candidates);
      break;
    case config::LRU:
      orderContainerByLRU(candidates);
      break;
    case config::FIFO:
      orderContainerByFIFO(candidates);
      break;
    case config::DEPENDENCIES:
      orderContainerByDependencies(candidates);
      break;
    case config::EXPLORER:
      orderContainerByRandomExplorer(candidates);
      break;
    default:
      DCHECK(false) << "Not implemented";
  }

  return candidates;
}

long long StorageCollection::estimatedStorageSize() const {
  return std::accumulate(storages.begin(), storages.end(), 0l,
                         [](long est, const auto& source) {
                           return est + source.second->estimatedSize();
                         });
}

void StorageCollection::orderContainerBySize(
    std::vector<std::pair<JSONStorage*, size_t>>& candidates) const {
  std::sort(candidates.begin(), candidates.end(),
            [](const std::pair<JSONStorage*, size_t>& a,
               const std::pair<JSONStorage*, size_t>& b) {
              return (a.second > b.second);
            });
}

void StorageCollection::orderContainerByLRU(
    std::vector<std::pair<JSONStorage*, size_t>>& candidates) const {
  std::sort(candidates.begin(), candidates.end(),
            [](const std::pair<JSONStorage*, size_t>& a,
               const std::pair<JSONStorage*, size_t>& b) {
              return (a.first->getLastUsed() < b.first->getLastUsed());
            });
}

void StorageCollection::orderContainerByFIFO(
    std::vector<std::pair<JSONStorage*, size_t>>& candidates) const {
  std::sort(candidates.begin(), candidates.end(),
            [&](const std::pair<JSONStorage*, size_t>& a,
                const std::pair<JSONStorage*, size_t>& b) {
              const auto& a_insert =
                  std::find(storageInsertion.begin(), storageInsertion.end(),
                            a.first->getName());
              const auto& b_insert =
                  std::find(storageInsertion.begin(), storageInsertion.end(),
                            b.first->getName());
              return a_insert < b_insert;
            });
}

void StorageCollection::orderContainerByDependencies(
    std::vector<std::pair<JSONStorage*, size_t>>& candidates) const {
  std::sort(candidates.begin(), candidates.end(),
            [&](const std::pair<JSONStorage*, size_t>& a,
                const std::pair<JSONStorage*, size_t>& b) {
              int a_dependCount = 0;
              int b_dependCount = 0;
              for (const auto& dependency : dependencies) {
                if (dependency.second.get() == a.first) {
                  a_dependCount++;
                }
                if (dependency.second.get() == b.first) {
                  b_dependCount++;
                }
              }
              return a_dependCount < b_dependCount;
            });
}

void StorageCollection::addDependency(
    const std::shared_ptr<JSONStorage>& store,
    const std::shared_ptr<JSONStorage>& dependson) {
  std::lock_guard<std::mutex> guard(mut);
  dependencies.insert({store, dependson});
}

std::vector<JSONStorage*> StorageCollection::getDependencies(
    const std::shared_ptr<JSONStorage>& store) const {
  std::vector<JSONStorage*> except{store.get()};
  std::vector<JSONStorage*> todo = {store.get()};
  // TODO: WARNING! Cyclic dependencies will kill this
  while (!todo.empty()) {
    const auto& s = todo.back();
    todo.pop_back();
    for (const auto& dependency : dependencies) {
      if (dependency.first.get() == s) {
        except.emplace_back(dependency.second.get());
        todo.emplace_back(dependency.second.get());
      }
    }
  }
  return except;
}

size_t StorageCollection::estimateHierarchySize(
    const std::shared_ptr<JSONStorage>& store) const {
  if (store == nullptr) {
    return 0;
  }
  std::vector<JSONStorage*> todo = {store.get()};
  size_t size = 0;
  while (!todo.empty()) {
    const auto& s = todo.back();
    todo.pop_back();
    if (s != nullptr) {
      size += s->estimatedSize();
    }
    for (const auto& dependency : dependencies) {
      if (dependency.first.get() == s) {
        todo.emplace_back(dependency.second.get());
      }
    }
  }
  return size;
}

size_t StorageCollection::parsedHierarchySize(
    const std::shared_ptr<JSONStorage>& store) const {
  if (store == nullptr) {
    return 0;
  }
  std::vector<JSONStorage*> todo = {store.get()};
  size_t size = 0;
  while (!todo.empty()) {
    const auto& s = todo.back();
    todo.pop_back();
    if (s != nullptr) {
      size += s->parsedSize();
    }
    for (const auto& dependency : dependencies) {
      if (dependency.first.get() == s) {
        todo.emplace_back(dependency.second.get());
      }
    }
  }
  return size;
}

size_t StorageCollection::estimatedSize() const {
  size_t size = 0;
  for (const auto& storage : storages) {
    size += storage.second->estimatedSize();
  }
  return size;
}

size_t StorageCollection::estimatedParsedSize() const {
  size_t size = 0;
  for (const auto& storage : storages) {
    size += storage.second->parsedSize();
  }
  return size;
}

void StorageCollection::orderContainerByRandomExplorer(
    std::vector<std::pair<JSONStorage*, size_t>>& candidates) const {
  if (candidates.empty()) {
    return;
  }
  double random_jump = 0.2;
  double go_back = 0.4;
  double stay = 0.4;

  orderContainerByLRU(candidates);
  const auto* current = candidates.back().first;

  std::vector<std::pair<JSONStorage*, std::pair<double, size_t>>> scores;
  scores.reserve(candidates.size());

  size_t max = 0;
  std::vector<double> random_jump_scores(1.0, candidates.size());
  for (size_t i = 0; i < random_jump_scores.size(); ++i) {
    random_jump_scores[random_jump_scores.size() - i - 1] /= i + 1;
  }
  double total_random_jump_score = std::accumulate(
      random_jump_scores.begin(), random_jump_scores.end(), 0.0);
  for (auto& jumpScore : random_jump_scores) {
    jumpScore /= total_random_jump_score * random_jump;
  }
  auto debug_val = std::accumulate(random_jump_scores.begin(),
                                   random_jump_scores.end(), 0.0);
  DCHECK(0.99 < debug_val && debug_val < 1.01) << "Should roughly equal 1";

  for (size_t i = 0; i < candidates.size(); ++i) {
    const auto& candidate = candidates[i];
    scores.emplace_back(candidate.first,
                        std::make_pair(random_jump_scores[i],
                                       candidate.first->estimatedSize()));

    // Add stay possibility for current node
    if (candidate.first == current) {
      scores.back().second.first += stay;
    }

    // Add go_back possibility for previous node
    if (std::find_if(
            dependencies.begin(), dependencies.end(),
            [&current](const std::pair<std::shared_ptr<JSONStorage>,
                                       std::shared_ptr<JSONStorage>>& p) {
              return p.first.get() == current;
            }) != dependencies.end()) {
      scores.back().second.first += go_back;
    }
  }

  for (auto& score : scores) {
    double relSize =
        static_cast<double>(score.first->size()) / static_cast<double>(max);
    score.second.first += random_jump * (relSize / scores.size());
  }
  debug_val = std::accumulate(scores.begin(), scores.end(), 0.0,
                              [](double score, const auto& source) {
                                return score + source.second.first;
                              });
  DCHECK(0.95 < debug_val && debug_val < 1.05) << "Should roughly equal 1";

  std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
    // If chance equal, sort by size
    if (a.second.first == b.second.first) {
      return a.second.second > b.second.second;
    }

    return (a.second.first < b.second.first);  // Sort by chance
  });
  std::vector<std::pair<JSONStorage*, size_t>> newCandidates;
  newCandidates.reserve(candidates.size());
  for (const auto& score : scores) {
    auto it = std::find_if(candidates.begin(), candidates.end(),
                           [&score](const std::pair<JSONStorage*, size_t>& p) {
                             return p.first == score.first;
                           });
    if (it != candidates.end()) {
      newCandidates.emplace_back(score.first, score.second.second);
    }
  }
  DCHECK(candidates.size() == newCandidates.size());
  candidates = std::move(newCandidates);
}