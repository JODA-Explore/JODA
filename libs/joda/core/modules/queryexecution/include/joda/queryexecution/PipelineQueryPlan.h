

#ifndef JODA_PIPELINE_QUERYPLAN_H
#define JODA_PIPELINE_QUERYPLAN_H

#include <joda/misc/Benchmark.h>
#include <joda/query/Query.h>
#include <joda/storage/collection/StorageCollection.h>

#include "../../../src/pipeline/Pipeline.h"

namespace joda::queryexecution {
/**
 * Class responsible for analyzing and executing a query.
 * The main logic of data destination/origin happens here.
 *
 */
class PipelineQueryPlan {
 public:
  /**
   * Initializes a new empty PipelineQueryPlan
   * @param max_threads The maximum number of threads the query plan may use
   */
  PipelineQueryPlan(size_t max_threads = config::storageRetrievalThreads);

  /**
   * @brief Creates a Pipeline for the given query object
   *
   * @param q The query to create the plan for
   * @param lastQuery If this query is the last query in the pipeline.
   */
  void createPlan(const std::shared_ptr<const joda::query::Query>& q,
                  bool lastQuery = true);

  /**
   * @brief Creates a Pipeline for the given query object
   *
   * @param queries A list of queries to create a plan for
   */
  void createPlan(
      const std::vector<std::shared_ptr<joda::query::Query>>& queries);

  /**
   * @brief Retrives the current Pipeline
   * @return The current Pipeline
   */
  const std::unique_ptr<pipeline::Pipeline>& getPipeline() const;

  /**
   * @brief Executes the current pipeline and retrieves the final resultID
   * @return The final resultID
   */
  unsigned long executeAndGetResult(Benchmark* bench = nullptr) const;

 protected:
  size_t max_threads = 1;

  // Pipeline
  std::unique_ptr<pipeline::Pipeline> p;

  // Current query
  std::shared_ptr<const joda::query::Query> current_q;
  bool currentIsStored = false;

  // Results
  unsigned long lastStorageID = JODA_STORE_SKIPPED_QUERY_ID;

  // Task Queue
  std::unordered_set<std::string> addedIDs;
  std::list<std::string> unfulfilledRequirements;
  std::vector<pipeline::tasks::PipelineTaskPtr> taskQueue;

  // Import Estimates
  size_t ls_size = 0;
  size_t stream_size = 0;

  // Timer
  RecurringTimer planning_timer;

  /**
   * Adds a task while resolving dependencies
   * @param task The task to add
   */
  void addTask(pipeline::tasks::PipelineTaskPtr&& task,
               bool instantResolveRequirement = true);

  /**
   * Commits all currently outstanding tasks to the pipeline
   */
  void commitTasks();

  /**
   * Resolves all outstanding task requirements
   */
  void resolveRequirements();
  /**
   * Resolves one specific requirement
   * @param requirement The requirement to resolve
   */
  void resolveRequirement(const std::string& requirement);

  /**
   * Resolves one specific prerequisite
   */
  void resolvePrerequisite(const std::string& prerequisite);

  /**
   * Plans the LOAD statement of the query
   * @return The storage into which will be imported and that will be used as
   * base
   */
  std::string planLOAD();

  /**
   * Plans the JOIN statement of the query
   */
  void planJOIN();

  /**
   * Plans the CHOOSE statement of the query
   */
  void planCHOOSE();

  /**
   * Plans the AS statement of the query
   */
  void planAS();

  /**
   * Plans the AGG statement of the query
   */
  void planAGG();

  /**
   * Plans the STORE statement of the query
   * @param lastQuery If this query is the last query in the pipeline. If true,
   * then results have to be exported
   * @return The potential storage to store in. If nullptr, the results have
   * been exported
   */
  std::string planSTORE(bool lastQuery = true);

  /**
   * @brief Creates a plan without STORE planning for usage as subqueries
   * 
   * @param q The query to plan
   * @return A list of all planned tasks
   */
  std::vector<pipeline::tasks::PipelineTaskPtr> createSubPlan(const std::shared_ptr<const joda::query::Query>& q);

  void addGenericBenchmarkInformation(Benchmark* bench) const;
};

}  // namespace joda::queryexecution

#endif  // JODA_PIPELINE_QUERYPLAN_H
