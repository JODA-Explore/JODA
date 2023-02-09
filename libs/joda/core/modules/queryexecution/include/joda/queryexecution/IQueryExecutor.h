//
// Created by Nico Schäfer on 9/27/17.
//

#ifndef JODA_IQUERYEXECUTOR_H
#define JODA_IQUERYEXECUTOR_H

#define NOT_APPLICABLE ULONG_MAX

#include "joda/container/JSONContainer.h"
namespace joda::query {
class Query;
}

class IQueryExecutor {
 public:
  explicit IQueryExecutor(){};

  virtual ~IQueryExecutor() = default;

  virtual unsigned long estimatedWork(const joda::query::Query &q,
                                      JSONContainer &cont) = 0;
  virtual std::shared_ptr<const DocIndex> execute(const joda::query::Query &q,
                                                  JSONContainer &cont) = 0;

  virtual std::string getName() const = 0;
  virtual void alwaysAfterSelect(const joda::query::Query &q,
                                 std::shared_ptr<const DocIndex> &sel,
                                 JSONContainer &cont) = 0;
  virtual std::unique_ptr<IQueryExecutor> duplicate() = 0;

 protected:
};

#endif  // JODA_IQUERYEXECUTOR_H
