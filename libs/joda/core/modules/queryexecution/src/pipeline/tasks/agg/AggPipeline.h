#ifndef JODA_PIPELINE_AGGPIPELINE_H
#define JODA_PIPELINE_AGGPIPELINE_H

#include <joda/container/JSONContainer.h>
#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/query/Query.h>
#include <joda/query/aggregation/IAggregator.h>

namespace joda::queryexecution::pipeline::tasks::agg {

/**
 * Performs the AGG operation on one container
 */
class AggExec {
 public:
  typedef queue::InputQueueTrait<std::shared_ptr<JSONContainer>> I;
  typedef queue::OutputQueueTrait<std::unique_ptr<query::IAggregator>> O;

  typedef I::Input Input;
  typedef O::Output Output;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1;

  AggExec(const std::shared_ptr<const query::Query>& query);
  AggExec(const AggExec& other);

  void fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff,
                  std::function<void(std::optional<Output>&)> sendPartial);

  void finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial);

  /**
   * @brief Aggregates a single container
   *
   * @param input The input container
   */
  void handleOne(I::Input& input);

 private:
  std::shared_ptr<const query::Query> query;
  std::vector<std::string> aggAttributes;

  // Aggregation
  RJMemoryPoolAlloc aggAlloc;
  std::vector<std::unique_ptr<query::IAggregator>> aggregators;
};

typedef IOExecutor<AggExec, tasks::AsyncType::MultiThreaded> AggPipeline;

}  // namespace joda::queryexecution::pipeline::tasks::agg

JODA_REGISTER_PIPELINE_TASK_IN_NS(AggPipeline, "AggPipeline",
                                  joda::queryexecution::pipeline::tasks::agg)

#endif  // JODA_PIPELINE_AGGPIPELINE_H