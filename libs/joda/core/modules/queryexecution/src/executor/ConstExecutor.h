//
// Created by Nico Schäfer on 12/13/17.
//

#ifndef JODA_CONSTEXECUTOR_H
#define JODA_CONSTEXECUTOR_H

#include "IQueryExecutor.h"
class ConstExecutor : public IQueryExecutor {
 public:
  unsigned long estimatedWork(const joda::query::Query &q, JSONContainer &cont) override;
  std::shared_ptr<const DocIndex> execute(const joda::query::Query &q, JSONContainer &cont) override;
  std::string getName() const override;
  void alwaysAfterSelect(const joda::query::Query &q, std::shared_ptr<const DocIndex> &sel, JSONContainer &cont) override;
  std::unique_ptr<IQueryExecutor> duplicate() override;
};

#endif //JODA_CONSTEXECUTOR_H
