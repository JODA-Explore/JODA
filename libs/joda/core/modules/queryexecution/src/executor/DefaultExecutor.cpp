//
// Created by Nico Schäfer on 9/27/17.
//

#include "DefaultExecutor.h"
#include "joda/query/Query.h"

DefaultExecutor::DefaultExecutor() : IQueryExecutor() {

}
unsigned long DefaultExecutor::estimatedWork(const joda::query::Query &q, JSONContainer &cont) {
  return cont.size();
}

std::shared_ptr<const DocIndex> DefaultExecutor::execute(const joda::query::Query &q, JSONContainer &cont) {
  std::function<bool(RapidJsonDocument &, size_t)> fun = [&](RapidJsonDocument &doc, size_t i) { return q.check(doc); };
  return cont.checkDocuments(fun);
}
std::string DefaultExecutor::getName() const {
  return "DefaultExecutor";
}
void DefaultExecutor::alwaysAfterSelect(const joda::query::Query &q,
                                        std::shared_ptr<const DocIndex> &sel,
                                        JSONContainer &cont) {

}
std::unique_ptr<IQueryExecutor> DefaultExecutor::duplicate() {
  return std::make_unique<DefaultExecutor>();
}
