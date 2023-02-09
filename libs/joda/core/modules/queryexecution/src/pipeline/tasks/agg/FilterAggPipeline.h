#ifndef JODA_PIPELINE_FILTERAGGPIPELINE_H
#define JODA_PIPELINE_FILTERAGGPIPELINE_H

#include <joda/container/JSONContainer.h>
#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/query/Query.h>
#include <joda/query/aggregation/IAggregator.h>

#include "../choose/ChoosePipeline.h"

namespace joda::queryexecution::pipeline::tasks::agg {

/**
 * Performs the AGG operation on one container
 */
class FilterAggExec {
 public:
  typedef queue::InputQueueTrait<choose::FilteredJSONContainer> I;
  typedef queue::OutputQueueTrait<std::unique_ptr<query::IAggregator>> O;

  typedef I::Input Input;
  typedef O::Output Output;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1;

  FilterAggExec(const std::shared_ptr<const query::Query>& query);
  FilterAggExec(const FilterAggExec& other);

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

typedef IOExecutor<FilterAggExec, tasks::AsyncType::MultiThreaded>
    FilterAggPipeline;

}  // namespace joda::queryexecution::pipeline::tasks::agg

JODA_REGISTER_PIPELINE_TASK_IN_NS(FilterAggPipeline, "FilterAggPipeline",
                                  joda::queryexecution::pipeline::tasks::agg)

#endif  // JODA_PIPELINE_FILTERAGGPIPELINE_H