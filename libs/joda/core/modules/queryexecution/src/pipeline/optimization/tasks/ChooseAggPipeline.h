#ifndef JODA_PIPELINE_CHOOSEAGG_PIPELINE_H
#define JODA_PIPELINE_CHOOSEAGG_PIPELINE_H

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/storage/JSONStorage.h>

#include <joda/queryexecution/IQueryExecutor.h>
#include "../../tasks/agg/FilterAggPipeline.h"
#include "../../tasks/choose/ChoosePipeline.h"

namespace joda::queryexecution::pipeline::tasks::optimization {

/**
 * Performs the CHOOSE and AGG operation on one container
 */
class ChooseAggExec {
 public:
  typedef queue::InputQueueTrait<std::shared_ptr<JSONContainer>> I;
  typedef queue::OutputQueueTrait<std::unique_ptr<query::IAggregator>> O;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1;

  ChooseAggExec(const std::vector<std::unique_ptr<IQueryExecutor>>& execs,
                  const std::shared_ptr<const query::Query>& query);
  ChooseAggExec(const ChooseAggExec& other);
  ChooseAggExec(ChooseAggExec&& other) = delete;

  void fillBuffer(std::optional<I::Input>& ibuff, std::optional<O::Output>& obuff,
                  std::function<void(std::optional<O::Output>&)> sendPartial);

  void finalize(std::optional<O::Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial);

  void handleOne(I::Input& input);

  std::string toString() const;

 private:
  choose::ChooseExec choose;
  agg::FilterAggExec agg;
};

typedef IOExecutor<ChooseAggExec, tasks::AsyncType::MultiThreaded>
    ChooseAggPipeline;

}  // namespace joda::queryexecution::pipeline::tasks::optimization

JODA_REGISTER_PIPELINE_TASK_IN_NS(
    ChooseAggPipeline, "ChooseAggPipeline",
    joda::queryexecution::pipeline::tasks::optimization)

#endif  // JODA_PIPELINE_CHOOSEAGG_PIPELINE_H