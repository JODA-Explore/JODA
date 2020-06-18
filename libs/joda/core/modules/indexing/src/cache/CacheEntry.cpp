//
// Created by Nico Schäfer on 8/2/17.
//

#include <utility>
#include "CacheEntry.h"

CacheEntry::CacheEntry(std::shared_ptr<const CacheIndex> docs, const std::shared_ptr<joda::query::Predicate> &predicate)
    : docs(std::move(docs)), predicate(predicate) {}

void CacheEntry::acceptPredicate(joda::query::PredicateVisitor &v) {
    predicate->accept(v);
}

std::shared_ptr<const CacheEntry::CacheIndex> CacheEntry::getDocs() const {
    return docs;
}

const long CacheEntry::estimatedSize() {
    return sizeof(CacheEntry) + docs->size()*sizeof(DOC_ID);
}

CacheEntry::CacheEntry(std::shared_ptr<const CacheIndex> &&docs,
                       const std::shared_ptr<joda::query::Predicate> &predicate) : docs(std::move(docs)), predicate(predicate) {

}
