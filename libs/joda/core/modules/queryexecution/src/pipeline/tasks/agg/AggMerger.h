#ifndef JODA_PIPELINE_AGGMERGER_H
#define JODA_PIPELINE_AGGMERGER_H

#include <joda/container/JSONContainer.h>
#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/query/aggregation/IAggregator.h>

#include <unordered_map>

namespace joda::queryexecution::pipeline::tasks::agg {

/**
 * Merges multiple Aggregators
 */
class AggMergerExec {
 public:
  typedef queue::InputQueueTrait<std::unique_ptr<query::IAggregator>> I;
  typedef queue::OutputQueueTrait<std::shared_ptr<JSONContainer>> O;

  typedef I::Input Input;
  typedef O::Output Output;

  AggMergerExec() = default;
  AggMergerExec(const AggMergerExec& other){};

  void fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff,
                  std::function<void(std::optional<Output>&)> sendPartial);

  void finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial);

 private:
  std::unordered_map<std::string, std::unique_ptr<query::IAggregator>>
      aggregators;
};

typedef IOExecutor<AggMergerExec, tasks::AsyncType::SingleThreaded> AggMerger;

}  // namespace joda::queryexecution::pipeline::tasks::agg

JODA_REGISTER_PIPELINE_TASK_IN_NS(AggMerger, "AggMerger",
                                  joda::queryexecution::pipeline::tasks::agg)

#endif  // JODA_PIPELINE_AGGMERGER_H