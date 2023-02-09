#ifndef JODA_PIPELINE_CHOOSEASAGG_PIPELINE_H
#define JODA_PIPELINE_CHOOSEASAGG_PIPELINE_H

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/storage/JSONStorage.h>

#include <joda/queryexecution/IQueryExecutor.h>
#include "../../tasks/agg/AggPipeline.h"
#include "../../tasks/as/AsPipeline.h"
#include "../../tasks/choose/ChoosePipeline.h"

namespace joda::queryexecution::pipeline::tasks::optimization {

/**
 * Performs the CHOOSE, AS, and AGG operation on one container
 */
class ChooseAsAggExec {
 public:
  typedef queue::InputQueueTrait<std::shared_ptr<JSONContainer>> I;
  typedef queue::OutputQueueTrait<std::unique_ptr<query::IAggregator>> O;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1;

  ChooseAsAggExec(const std::vector<std::unique_ptr<IQueryExecutor>>& execs,
                  const std::shared_ptr<const query::Query>& query);
  ChooseAsAggExec(const ChooseAsAggExec& other);
  ChooseAsAggExec(ChooseAsAggExec&& other) = delete;

  void fillBuffer(std::optional<I::Input>& ibuff, std::optional<O::Output>& obuff,
                  std::function<void(std::optional<O::Output>&)> sendPartial);

  void finalize(std::optional<O::Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial);

  void handleOne(I::Input& input);

  std::string toString() const;

 private:
  choose::ChooseExec choose;
  as::AsExec as;
  agg::AggExec agg;
};

typedef IOExecutor<ChooseAsAggExec, tasks::AsyncType::MultiThreaded>
    ChooseAsAggPipeline;

}  // namespace joda::queryexecution::pipeline::tasks::optimization

JODA_REGISTER_PIPELINE_TASK_IN_NS(
    ChooseAsAggPipeline, "ChooseAsAggPipeline",
    joda::queryexecution::pipeline::tasks::optimization)

#endif  // JODA_PIPELINE_CHOOSEASAGG_PIPELINE_H