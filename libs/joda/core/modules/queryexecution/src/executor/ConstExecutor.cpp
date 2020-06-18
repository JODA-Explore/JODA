//
// Created by Nico Schäfer on 12/13/17.
//

#include "ConstExecutor.h"
#include "joda/query/Query.h"

unsigned long ConstExecutor::estimatedWork(const joda::query::Query &q, JSONContainer &cont) {
  bool b;
  return q.chooseIsConst(b) ? 0 : NOT_APPLICABLE;
}

std::shared_ptr<const DocIndex> ConstExecutor::execute(const joda::query::Query &q, JSONContainer &cont) {
  bool all;
  auto applicable = q.chooseIsConst(all);
  assert(applicable && "Should not be chosen otherwise");
  if (all) {
    return cont.getAllIDs();
  } else {
    return nullptr;
  }
}
std::string ConstExecutor::getName() const {
  return "ConstExecutor";
}
void ConstExecutor::alwaysAfterSelect(const joda::query::Query &q,
                                      std::shared_ptr<const DocIndex> &sel,
                                      JSONContainer &cont) {

}
std::unique_ptr<IQueryExecutor> ConstExecutor::duplicate() {
  return std::make_unique<ConstExecutor>();
}
