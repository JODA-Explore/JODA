//
// Created by Nico Schäfer on 9/27/17.
//

#include "CacheExecutor.h"
#include "joda/config/config.h"

#include <joda/query/predicate/ToStringVisitor.h>
#include <utility>
#include "joda/query/Query.h"

CacheExecutor::CacheExecutor(const joda::query::Query& q) {
  joda::query::ToStringVisitor stringify;
  q.getPredicate()->accept(stringify);
  predStr = stringify.popString();
}

unsigned long CacheExecutor::estimatedWork(const joda::query::Query& q,
                                           JSONContainer& cont) {
  if (!config::queryCache) {
    return NOT_APPLICABLE;
  }
  // Check for complete matching cache
  if (cont.getCache()->cacheAvailable(predStr)){
    return 0;
  }

  // Check if root Pred is AND and check LHS and RHS:
  auto pred = q.getPredicate();
  auto *base = pred.get();
  auto *andpred = dynamic_cast<joda::query::AndPredicate*>(base);
  if (andpred == nullptr){
    return NOT_APPLICABLE;
  }

  auto &lhs = andpred->getLHS();
  auto lhscache = cont.getCache()->getBestCache(lhs);
  auto lhscacheSize = lhscache == nullptr ? NOT_APPLICABLE : std::count(lhscache->begin(), lhscache->end(), true);
  auto &rhs = andpred->getRHS();
  auto rhscache = cont.getCache()->getBestCache(rhs);
  auto rhscacheSize = rhscache == nullptr ? NOT_APPLICABLE : std::count(rhscache->begin(), rhscache->end(), true);

  if (lhscacheSize <= rhscacheSize){
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

  auto pred = q.getPredicate();
  auto *base = pred.get();
  auto *andpred = dynamic_cast<joda::query::AndPredicate*>(base);
  DCHECK(andpred != nullptr) << "This should not be possible, as else NOT_APPLICABLE would have been returned";

  auto &lhs = andpred->getLHS();
  auto lhscache = cont.getCache()->getBestCache(lhs);
  auto lhscacheSize = lhscache == nullptr ? NOT_APPLICABLE : std::count(lhscache->begin(), lhscache->end(), true);
  auto &rhs = andpred->getRHS();
  auto rhscache = cont.getCache()->getBestCache(rhs);
  auto rhscacheSize = rhscache == nullptr ? NOT_APPLICABLE : std::count(rhscache->begin(), rhscache->end(), true);
  if (lhscacheSize <= rhscacheSize){
    // Check RHS predicate against LHS cache
    std::function<bool(RapidJsonDocument&, size_t)> fun =
      [&](RapidJsonDocument& doc, size_t i) { return rhs->check(doc); };
    auto res = cont.forAllRet<std::function<bool(RapidJsonDocument&, size_t)>,bool>(fun,*lhscache);
    return std::make_shared<const DocIndex>(res);
  }
  // Check LHS predicate against RHS cache
  std::function<bool(RapidJsonDocument&, size_t)> fun =
      [&](RapidJsonDocument& doc, size_t i) { return lhs->check(doc); };
  auto res = cont.forAllRet<std::function<bool(RapidJsonDocument&, size_t)>,bool>(fun,*rhscache);
  return std::make_shared<const DocIndex>(res);
}

std::string CacheExecutor::getName() const { return "CacheExecutor"; }

void CacheExecutor::alwaysAfterSelect(const joda::query::Query& q,
                                      std::shared_ptr<const DocIndex>& sel,
                                      JSONContainer& cont) {
  /*
   * Set Cache
   */
  auto queryPredicate = q.getPredicate();
  cont.getCache()->addQueryResult(sel, q.getPredicate());
}

std::unique_ptr<IQueryExecutor> CacheExecutor::duplicate() {
  return std::unique_ptr<CacheExecutor>(new CacheExecutor(predStr));
}

CacheExecutor::CacheExecutor(std::string predStr)
    : predStr(std::move(predStr)) {}
