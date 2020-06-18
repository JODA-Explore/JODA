//
// Created by Nico Schäfer on 8/2/17.
//

#ifndef JODA_CACHEENTRY_H
#define JODA_CACHEENTRY_H

#include <unordered_set>
#include <joda/document/RapidJsonDocument.h>
#include <joda/query/predicate/PredicateVisitor.h>
#include <joda/query/predicate/Predicate.h>

/**
 * One Cache entry, containing the result documents and the predicate responsible for them
 */
class CacheEntry {
 public:
  typedef std::vector<bool> CacheIndex;
  CacheEntry(std::shared_ptr<const CacheIndex> docs, const std::shared_ptr<joda::query::Predicate> &predicate);
  CacheEntry(std::shared_ptr<const CacheIndex> &&docs, const std::shared_ptr<joda::query::Predicate> &predicate);

  /**
   * Executes the given PredicateVisitor on the stored predicate
   * @param v A reference to the PredicateVisitor to execute
   */
  void acceptPredicate(joda::query::PredicateVisitor &v);

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
  std::shared_ptr<joda::query::Predicate> predicate;
};

#endif //JODA_CACHEENTRY_H
