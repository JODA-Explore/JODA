//
// Created by "Nico Schaefer" on 1/17/17.
//

#include "../../include/joda/indexing/QueryCache.h"
#include <joda/query/predicate/ToStringVisitor.h>
#include <iostream>

void QueryCache::addQueryResult(std::unique_ptr<CacheEntry>&& ce) {
  joda::query::ToStringVisitor stringify;
  ce->acceptPredicate(stringify);
  auto str = stringify.popString();
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
    const std::shared_ptr<joda::query::Predicate>& predicate) {
  addQueryResult(std::make_unique<CacheEntry>(docs, predicate));
}

std::shared_ptr<const CacheEntry::CacheIndex> QueryCache::getBestCache(
    const std::shared_ptr<joda::query::Predicate>& pred) {
  if (queryMap.empty()) {
    return nullptr;  // No need to stringify if cache is empty
  }
  joda::query::ToStringVisitor stringify;
  pred->accept(stringify);
  auto str = stringify.popString();
  return getBestCache(str);
}

std::shared_ptr<const CacheEntry::CacheIndex> QueryCache::getBestCache(
    const std::unique_ptr<joda::query::Predicate>& pred) {
  if (queryMap.empty()) {
    return nullptr;  // No need to stringify if cache is empty
  }
  joda::query::ToStringVisitor stringify;
  pred->accept(stringify);
  auto str = stringify.popString();
  return getBestCache(str);
}

void QueryCache::reset() { queryMap.clear(); }

bool QueryCache::cacheAvailable(
    const std::shared_ptr<joda::query::Predicate>& pred) {
  if (queryMap.empty()) {
    return false;  // No need to stringify if cache is empty
  }
  joda::query::ToStringVisitor stringify;
  pred->accept(stringify);
  auto str = stringify.popString();
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
