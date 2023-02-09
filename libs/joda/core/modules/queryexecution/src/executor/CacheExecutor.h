//
// Created by Nico Schäfer on 9/27/17.
//

#ifndef JODA_CACHEEXECUTOR_H
#define JODA_CACHEEXECUTOR_H

#include <joda/queryexecution/IQueryExecutor.h>

class CacheExecutor : public IQueryExecutor {
 public:
  explicit CacheExecutor(const joda::query::Query &q);
  unsigned long estimatedWork(const joda::query::Query &q,
                              JSONContainer &cont) override;
  std::shared_ptr<const DocIndex> execute(const joda::query::Query &q,
                                          JSONContainer &cont) override;
  std::string getName() const override;
  void alwaysAfterSelect(const joda::query::Query &q,
                         std::shared_ptr<const DocIndex> &sel,
                         JSONContainer &cont) override;
  std::unique_ptr<IQueryExecutor> duplicate() override;

 protected:
  CacheExecutor(std::string predStr);
  std::string predStr;
};

#endif  // JODA_CACHEEXECUTOR_H
