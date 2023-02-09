#ifndef JODA_WINDOW_AGGREGATOR_H
#define JODA_WINDOW_AGGREGATOR_H

#include <joda/container/JSONContainer.h>
#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/query/Query.h>
#include <joda/query/aggregation/IAggregator.h>

namespace joda::queryexecution::pipeline::tasks::agg {

/**
 * Performs the AGG operation on one container and sends the result depending on
 * a tumbling window
 */
class WindowAggExec {
 public:
  typedef queue::InputQueueTrait<std::shared_ptr<JSONContainer>> I;
  typedef queue::OutputQueueTrait<std::shared_ptr<JSONContainer>> O;

  typedef I::Input Input;
  typedef O::Output Output;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1;
  // Overwrite Output Bulk Size
  static constexpr size_t OutputBulk = 1;

  WindowAggExec(const std::shared_ptr<const query::Query>& query,
                uint64_t windowSize);
  WindowAggExec(const WindowAggExec& other);

  void fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff,
                  std::function<void(std::optional<Output>&)> sendPartial);

  void finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial);


 private:
  std::shared_ptr<const query::Query> query;
  std::vector<std::string> aggAttributes;
  uint64_t windowSize;
  uint64_t aggregated = 0;

  // Aggregation
  RJMemoryPoolAlloc aggAlloc;
  std::vector<std::unique_ptr<query::IAggregator>> originalAggregators;
  std::vector<std::unique_ptr<query::IAggregator>> currentAggregators{};

  void cloneAggregators();
  std::shared_ptr<JSONContainer> finalizeCurrentAggregators();
};

typedef IOExecutor<WindowAggExec, tasks::AsyncType::SingleThreaded> WindowAgg;

}  // namespace joda::queryexecution::pipeline::tasks::agg

JODA_REGISTER_PIPELINE_TASK_IN_NS(WindowAgg, "WindowAgg",
                                  joda::queryexecution::pipeline::tasks::agg)

#endif  // JODA_WINDOW_AGGREGATOR_H