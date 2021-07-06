//
// Created by Nico Schäfer on 11/7/17.
//

#ifndef JODA_QUERYTHREAD_H
#define JODA_QUERYTHREAD_H

#include <joda/concurrency/IOThreadPool.h>
#include <joda/misc/Benchmark.h>
#include <joda/query/Query.h>
#include <joda/query/aggregation/IAggregator.h>

#include "executor/IQueryExecutor.h"

/**
 * QueryThreadConfig contains all the data which should be shared between
 * QueryThread s. It mostly consits of the query to execute and previously
 * extracted information to reduce runtime cost.
 */
struct QueryThreadConfig {
  std::shared_ptr<joda::query::Query> q;
  Benchmark *bench;
  std::vector<std::unique_ptr<IQueryExecutor>> executors;
  std::vector<std::unique_ptr<joda::query::IAggregator>> aggregators;
  std::shared_ptr<JoinManager> joinManager;
  joda::query::AggregatorQueue::queue_t *aggQueue{};
  std::string tmpdir;

  QueryThreadConfig() = default;

  QueryThreadConfig(std::shared_ptr<joda::query::Query> &q)
      : q(q),
        bench(nullptr),
        executors(),
        aggregators(),
        joinManager(q->getStoreJoinManager()) {
    for (const auto &agg : this->q->getAggregators()) {
      aggregators.emplace_back(agg->duplicate());
    }
  }

  QueryThreadConfig(const QueryThreadConfig &o)
      : q(o.q),
        bench(o.bench),
        executors(),
        aggregators(),
        joinManager(o.joinManager),
        aggQueue(o.aggQueue),
        tmpdir(o.tmpdir) {
    for (const auto &exec : o.executors) {
      executors.emplace_back(exec->duplicate());
    }
    for (const auto &agg : o.aggregators) {
      aggregators.emplace_back(agg->duplicate());
    }
  }

  void addExecutor(std::unique_ptr<IQueryExecutor> &&exec) {
    executors.emplace_back(std::move(exec));
  }
};

/**
 * The QueryThread class is responsible for actually executing a Query on one or
 * multiple containers. It reads containers from an input queue, executes the
 * query on each container read and then writes the output container to an
 * output queue.
 */
class QueryThread
    : public IWorkerThread<JsonContainerRefQueue, JsonContainerQueue,
                           QueryThreadConfig> {
 public:
  /**
   * Initializes a new QueryThread
   */
  QueryThread(IQueue *iqueue, OQueue *oqueue, WConf &conf);
  ~QueryThread() override;

  typedef IPayload ContRef;
  typedef OPayload OwnedCont;

  /**
   * Returns a unique ID identifying this querythread
   * @return
   */
  const std::string getThreadID() const;

  /*
   * Aggregation
   */

  /**
   * Checks whether aggregations have to be performed
   * @return true if yes, false if not
   */
  bool hasAggregators() const;

 protected:
  bool hasToProject() const;
  bool canCreateView() const;

  /*
   * Functions
   */
  /**
   * Selects the documents adhering to he filter (CHOOSE) step in the query.
   * @param cont The container to perform the selection on
   * @return A DocIndex containing information on which documents are selected
   */
  std::shared_ptr<const DocIndex> select(JSONContainer &cont) const;

  /**
   * Transforms the the selected documents in the given container
   * @param cont The container to perform the projection on
   * @param ids A DocIndex containing information on which documents are to be
   * projected
   * @param alloc An allocator to use for projection. This reduces memory
   * consumption and runtime required by many allocation operations
   * @return A list of all documents obtained by transforming the input
   * documents
   */
  std::vector<std::unique_ptr<RJDocument>> defaultProject(
      JSONContainer &cont, const DocIndex &ids, RJMemoryPoolAlloc &alloc) const;

  /**
   * Aggregates the given list of documents.
   * The results are stored in the aggregator classed within the config objects
   * @param docs The documents to aggregate
   */
  void aggregate(const std::vector<RapidJsonDocument> &docs) const;

  /**
   * Aggregates all documents within the container.
   * The results are stored in the aggregator classed within the config objects
   * @param pipelineCont The container having the documents
   * @param selectResult The index showing which documents have to be aggregated
   * @param isSelected flag dictating if selectResult should be used or not.
   */
  void aggregate(ContRef pipelineCont,
                 const std::shared_ptr<const DocIndex> &selectResult,
                 bool isSelected) const;

  void work() override;

  void logTimers() const;
  RecurringTimer bloom_timer;
  RecurringTimer select_timer;
  RecurringTimer project_timer;
  RecurringTimer aggregate_timer;
  RecurringTimer copy_timer;
  RecurringTimer serialize_timer;
  RecurringTimer sample_view_cost_timer;
};

#endif  // JODA_QUERYTHREAD_H
