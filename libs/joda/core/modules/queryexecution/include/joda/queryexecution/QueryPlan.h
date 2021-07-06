//
// Created by Nico Schäfer on 4/25/17.
//

#ifndef JODA_QUERYPLAN_H
#define JODA_QUERYPLAN_H

#include <joda/misc/Benchmark.h>
#include <joda/parser/ReaderParser.h>
#include <joda/query/Query.h>
#include <joda/query/aggregation/IAggregator.h>
#include <joda/storage/JSONStorage.h>
struct ExecutionStats {
  size_t parsedDocs;
  size_t parsedConts;
  size_t numConts;
  size_t evaluatedConts;
};

/**
 * Class responsible for analyzing and executing a query.
 * The main logic of data destination/origin happens here.
 *
 */
class QueryPlan {
 public:
  /**
   * Initializes a new QueryPlan with a given query.
   * The query is already analyzed in the constructor for data origin and
   * destination
   * @param q The query to execute later on
   */
  QueryPlan(const std::shared_ptr<joda::query::Query> &q);

  /**
   * Executes the query.
   * The query result is stored in an collection, which will be registered at
   * the StorageCollection as a temporary result. The ID of this result is then
   * returned after the query completed
   * @param bench an optional Benchmark, which is used to time the query
   * execution. If null, no timing is performed
   * @return A ID representing the result set.
   */
  unsigned long executeQuery(Benchmark *bench = nullptr);

  /**
   * Returns realtime ExecutionStats for UI query-progress reports
   * @return
   */
  ExecutionStats getStats() const;

  /**
   * Checks if the queryplan has to parse files from outside the system.
   * @return true, if it has to, false if not
   */
  bool hasToParse() const;

 protected:
  unsigned long resultID = 0;
  unsigned long maxThreads = 0;
  bool skip = false;

  void addGenericBenchmarkInformation(Benchmark *bench = nullptr) const;
  std::shared_ptr<JSONStorage> load;
  std::shared_ptr<joda::query::Query> q;
  std::unique_ptr<JSONContainer> aggregate(
      std::shared_ptr<joda::query::Query> &q,
      joda::query::AggregatorQueue::queue_t *queue, Benchmark *benchmark);
  std::unique_ptr<JsonContainerRefQueue::queue_t> loadQueue;
  std::unique_ptr<JsonContainerQueue::queue_t> storeQueue;
  std::unique_ptr<joda::query::AggregatorQueue::queue_t> aggregatorQueue;
  joda::docparsing::ReaderParser parser;
};

#endif  // JODA_QUERYPLAN_H
