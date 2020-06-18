//
// Created by Nico Schäfer on 9/27/17.
//

#include <joda/query/predicate/ToStringVisitor.h>
#include "CacheExecutor.h"
#include "joda/query/Query.h"
#include "joda/config/config.h"


CacheExecutor::CacheExecutor(const joda::query::Query &q) : IQueryExecutor() {
  joda::query::ToStringVisitor stringify;
  q.getPredicate()->accept(stringify);
  predStr = stringify.popString();
}

unsigned long CacheExecutor::estimatedWork(const joda::query::Query &q, JSONContainer &cont) {
  if (!config::queryCache) return NOT_APPLICABLE;
  auto ret = cont.getCache()->cacheAvailable(predStr) ? 0 : NOT_APPLICABLE;
  return ret;
}

std::shared_ptr<const DocIndex> CacheExecutor::execute(const joda::query::Query &q, JSONContainer &cont) {
  /*
 * Get Cache
 */

  return cont.getCache()->getBestCache(predStr);
}

std::string CacheExecutor::getName() const {
  return "CacheExecutor";
}

void CacheExecutor::alwaysAfterSelect(const joda::query::Query &q,
                                      std::shared_ptr<const DocIndex> &sel,
                                      JSONContainer &cont) {
  /*
   * Set Cache
   */
  auto queryPredicate = q.getPredicate();
  cont.getCache()->addQueryResult(sel, q.getPredicate());
}

std::unique_ptr<IQueryExecutor> CacheExecutor::duplicate() {
  return std::unique_ptr<CacheExecutor>(new CacheExecutor(predStr));
}

CacheExecutor::CacheExecutor(const std::string &predStr) : predStr(predStr) {

}
