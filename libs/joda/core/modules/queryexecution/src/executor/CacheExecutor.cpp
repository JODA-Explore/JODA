//
// Created by Nico Schäfer on 9/27/17.
//

#include "CacheExecutor.h"

#include <joda/query/values/BinaryBoolProvider.h>

#include <utility>

#include "joda/config/config.h"
#include "joda/query/Query.h"

CacheExecutor::CacheExecutor(const joda::query::Query& q) {
  predStr = q.getChoose()->toString();
}

unsigned long CacheExecutor::estimatedWork(const joda::query::Query& q,
                                           JSONContainer& cont) {
  if (!config::queryCache) {
    return NOT_APPLICABLE;
  }
  // Check for complete matching cache
  if (cont.getCache()->cacheAvailable(predStr)) {
    return 0;
  }

  // Check if root Pred is AND and check LHS and RHS:
  auto pred = q.getChoose();
  auto* base = pred.get();
  auto* andpred = dynamic_cast<joda::query::AndProvider*>(base);
  if (andpred == nullptr) {
    return NOT_APPLICABLE;
  }

  const auto& lhs = andpred->getLhs();
  auto lhscache = cont.getCache()->getBestCache(lhs);
  auto lhscacheSize = lhscache == nullptr ? NOT_APPLICABLE
                                          : lhscache->count();
  const auto& rhs = andpred->getRhs();
  auto rhscache = cont.getCache()->getBestCache(rhs);
  auto rhscacheSize = rhscache == nullptr ? NOT_APPLICABLE
                                          : rhscache->count();

  if (lhscacheSize <= rhscacheSize) {
    return lhscacheSize;
  }
  return rhscacheSize;
}

std::shared_ptr<const DocIndex> CacheExecutor::execute(
    const joda::query::Query& q, JSONContainer& cont) {
  /*
   * Get Cache
   */
  auto wholecache = cont.getCache()->getBestCache(predStr);
  if (wholecache != nullptr) {
    return wholecache;
  }

  auto pred = q.getChoose();
  auto* base = pred.get();
  auto* andpred = dynamic_cast<joda::query::AndProvider*>(base);
  DCHECK(andpred != nullptr) << "This should not be possible, as else "
                                "NOT_APPLICABLE would have been returned";

  auto& lhs = andpred->getLhs();
  auto lhscache = cont.getCache()->getBestCache(lhs);
  auto lhscacheSize = lhscache == nullptr ? NOT_APPLICABLE
                                          : lhscache->count();
  auto& rhs = andpred->getRhs();
  auto rhscache = cont.getCache()->getBestCache(rhs);
  auto rhscacheSize = rhscache == nullptr ? NOT_APPLICABLE
                                          : rhscache->count();
  RJMemoryPoolAlloc alloc;
  DocIndex ind(cont.size());
  if (lhscacheSize <= rhscacheSize) {
    // Check RHS predicate against LHS cache
    std::function<void(RapidJsonDocument&, size_t)> fun =
        [&rhs,&alloc, &ind](RapidJsonDocument& doc, size_t i) {
          if (!rhs->isAtom()) {
            auto ret = rhs->getValue(doc, alloc);
            if (ret == nullptr) {
              ind[i] = false;
              return;
            }
            if (!ret->IsBool()) {
              ind[i] = false;
              return;
            }
            ind[i] = ret->GetBool();
          } else {
            auto ret = rhs->getAtomValue(doc, alloc);
            if (!ret.IsBool()) {
              ind[i] = false;
              return;
            }
            ind[i] = ret.GetBool();
          }
        };
    cont.forAll(
            fun, *lhscache);
    return std::make_shared<const DocIndex>(std::move(ind));
  }
  // Check LHS predicate against RHS cache
  std::function<void(RapidJsonDocument&, size_t)> fun =
      [&lhs,&alloc,&ind](RapidJsonDocument& doc, size_t i) {
        if (!lhs->isAtom()) {
          auto ret = lhs->getValue(doc, alloc);
          if (ret == nullptr) {
            ind[i] = false;
            return;
          }
          if (!ret->IsBool()) {
            ind[i] = false;
            return;
          }
          ind[i] = ret->GetBool();
        } else {
          auto ret = lhs->getAtomValue(doc, alloc);
          if (!ret.IsBool()) {
            ind[i] = false;
            return;
          }
          ind[i] = ret.GetBool();
        }
      };
    cont.forAll(
            fun, *rhscache);
    return std::make_shared<const DocIndex>(std::move(ind));
}

std::string CacheExecutor::getName() const { return "CacheExecutor"; }

void CacheExecutor::alwaysAfterSelect(const joda::query::Query& q,
                                      std::shared_ptr<const DocIndex>& sel,
                                      JSONContainer& cont) {
  /*
   * Set Cache
   */
  auto queryPredicate = q.getChoose();
  cont.getCache()->addQueryResult(sel, queryPredicate);
}

std::unique_ptr<IQueryExecutor> CacheExecutor::duplicate() {
  return std::unique_ptr<CacheExecutor>(new CacheExecutor(predStr));
}

CacheExecutor::CacheExecutor(std::string predStr)
    : predStr(std::move(predStr)) {}
