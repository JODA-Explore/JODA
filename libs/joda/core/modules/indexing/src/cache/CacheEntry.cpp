//
// Created by Nico Schäfer on 8/2/17.
//

#include "CacheEntry.h"
#include <utility>

CacheEntry::CacheEntry(std::shared_ptr<const CacheIndex> docs,
                       std::unique_ptr<joda::query::IValueProvider>&& predicate)
    : docs(std::move(docs)), predicate(std::move(predicate)) {}

const std::unique_ptr<joda::query::IValueProvider>& CacheEntry::getPredicate() const{
  return predicate;
}

std::shared_ptr<const CacheEntry::CacheIndex> CacheEntry::getDocs() const {
  return docs;
}

const long CacheEntry::estimatedSize() {
  return sizeof(CacheEntry) + docs->size() * sizeof(DOC_ID);
}

CacheEntry::CacheEntry(std::shared_ptr<const CacheIndex>&& docs,
                       std::unique_ptr<joda::query::IValueProvider>&& predicate)
    : docs(std::move(docs)), predicate(std::move(predicate)) {}
