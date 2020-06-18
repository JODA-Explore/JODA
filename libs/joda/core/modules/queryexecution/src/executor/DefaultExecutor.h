//
// Created by Nico Schäfer on 9/27/17.
//

#ifndef JODA_DEFAULTEXECUTOR_H
#define JODA_DEFAULTEXECUTOR_H

#include "IQueryExecutor.h"
class DefaultExecutor : public IQueryExecutor{
 public:
  explicit DefaultExecutor();
  unsigned long estimatedWork(const joda::query::Query &q, JSONContainer &cont) override;
  std::shared_ptr<const DocIndex> execute(const joda::query::Query &q, JSONContainer &cont) override;
  std::string getName() const override;
  void alwaysAfterSelect(const joda::query::Query &q, std::shared_ptr<const DocIndex> &sel, JSONContainer &cont) override;
  std::unique_ptr<IQueryExecutor> duplicate() override;
};

#endif //JODA_DEFAULTEXECUTOR_H
