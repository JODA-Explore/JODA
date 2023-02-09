//
// Created by "Nico Schaefer" on 1/17/17.
//

#include "../../include/joda/indexing/QueryCache.h"
#include <iostream>

void QueryCache::addQueryResult(std::unique_ptr<CacheEntry>&& ce) {
  auto str = ce->getPredicate()->toString();
  auto cache = queryMap.find(str);

  // If there is a cache with same hash
  if (cache != queryMap.end()) {
    // given result is newer, delete old and replace it with new
    return;
  }  // If it does not exist, also add it
  this->queryMap.emplace(str, std::move(ce));
}

void QueryCache::addQueryResult(
    std::shared_ptr<const CacheEntry::CacheIndex> docs,
    const std::unique_ptr<joda::query::IValueProvider>& predicate) {
  addQueryResult(std::make_unique<CacheEntry>(docs, predicate->duplicate()));
}


std::shared_ptr<const CacheEntry::CacheIndex> QueryCache::getBestCache(
    const std::unique_ptr<joda::query::IValueProvider>& pred) {
  if (queryMap.empty()) {
    return nullptr;  // No need to stringify if cache is empty
  }
  auto str = pred->toString();
  return getBestCache(str);
}

void QueryCache::reset() { queryMap.clear(); }

bool QueryCache::cacheAvailable(
    const std::unique_ptr<joda::query::IValueProvider>& pred) {
  if (queryMap.empty()) {
    return false;  // No need to stringify if cache is empty
  }
  auto str = pred->toString();
  return cacheAvailable(str);
}

bool QueryCache::cacheAvailable(const std::string& predStr) {
  if (queryMap.empty()) {
    return false;  // No need to stringify if cache is empty
  }
  auto cache = queryMap.find(predStr);
  // If there is a cache with same hash
  return cache != queryMap.end();
}

std::shared_ptr<const CacheEntry::CacheIndex> QueryCache::getBestCache(
    const std::string& predStr) {
  auto cache = queryMap.find(predStr);

  // If there is a cache with same hash
  if (cache != queryMap.end()) {
    return cache->second->getDocs();
  }
  return nullptr;
}
