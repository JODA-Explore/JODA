//
// Created by "Nico Schaefer" on 1/17/17.
//

#ifndef JODA_QUERYCACHE_H
#define JODA_QUERYCACHE_H

#include <unordered_map>
#include <joda/document/RapidJsonDocument.h>
#include <joda/query/predicate/Predicate.h>
#include "../../../src/cache/CacheEntry.h"

/**
 * This class is responsible for managing a cache for previously exectuted queries.
 */
class QueryCache {


 public:
  /**
   * Returns the cached DOC_IDs for the given predicate
   * @param pred The predicated in internal representation
   * @return A set of DOC_IDs stored by a previous query execution
   */
  std::shared_ptr<const CacheEntry::CacheIndex> getBestCache(const std::shared_ptr<joda::query::Predicate> &pred);
  /**
   * Returns the cached DOC_IDs for the given predicate
   * @param predStr The predicated in string representation
   * @return A set of DOC_IDs stored by a previous query execution
   */
  std::shared_ptr<const CacheEntry::CacheIndex> getBestCache(const std::string &predStr);
  /**
   * Checks if the given predicate is stored in the cache
   * @param pred Predicate to check for
   * @return True if a cache exists; False else
   */
  bool cacheAvailable(const std::shared_ptr<joda::query::Predicate> &pred);
  /**
   * Checks if the given predicate is stored in the cache
   * @param predStr Predicate to check for
   * @return True if a cache exists; False else
   */
  bool cacheAvailable(const std::string &predStr);
  /**
   * Adds an already created CacheEntry
   * @param ce CacheEntry to insert to the QueryCache
   */
  void addQueryResult(std::unique_ptr<CacheEntry> &&ce);
  /**
   * Creates and adds an CacheEntry to the QueryCache
   * @param docs The set of DOC_IDs to add to the CacheEntry
   * @param predicate The predicate that produced docs
   */
  void addQueryResult(std::shared_ptr<const CacheEntry::CacheIndex> docs,
                      const std::shared_ptr<joda::query::Predicate> &predicate);

  /**
   * Resets the QueryCache and removes all CacheEntries
   */
  void reset();

 protected:

  std::unordered_map<std::string, std::unique_ptr<CacheEntry>> queryMap;

};

#endif //JODA_QUERYCACHE_H
