//
// Created by Nico Schäfer on 8/2/17.
//

#ifndef JODA_CACHEENTRY_H
#define JODA_CACHEENTRY_H

#include <joda/document/RapidJsonDocument.h>
#include <joda/query/values/IValueProvider.h>

#include <unordered_set>
#include <boost/dynamic_bitset.hpp>


/**
 * One Cache entry, containing the result documents and the predicate
 * responsible for them
 */
class CacheEntry {
 public:
  using CacheIndex = boost::dynamic_bitset<>;
  
  CacheEntry(std::shared_ptr<const CacheIndex> docs,
             std::unique_ptr<joda::query::IValueProvider>&& predicate);
  CacheEntry(std::shared_ptr<const CacheIndex>&& docs,
             std::unique_ptr<joda::query::IValueProvider>&& predicate);

  /**
   * Returns the used predicate
   */
  const std::unique_ptr<joda::query::IValueProvider>& getPredicate() const;

  /**
   * Returns the set of documents in the CacheEntry
   * @return unordered_set of documents in the CacheEntry
   */
  std::shared_ptr<const CacheIndex> getDocs() const;

  /**
   * Returns the estimated memory size of the CacheEntry
   * @return estimated memory size of the CacheEntry
   */
  const long estimatedSize();

 protected:
  std::shared_ptr<const CacheIndex> docs;
  std::unique_ptr<joda::query::IValueProvider> predicate;
};

#endif  // JODA_CACHEENTRY_H
