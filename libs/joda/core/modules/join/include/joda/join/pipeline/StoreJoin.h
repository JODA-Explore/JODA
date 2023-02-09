#ifndef JODA_PIPELINE_STOREJOIN_H
#define JODA_PIPELINE_STOREJOIN_H

#include <joda/join/JoinManager.h>
#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
namespace joda::queryexecution::pipeline::tasks::join {

/**
 * Joins all containers passed to this task
 */
class StoreJoinExec {
 public:
  typedef queue::InputQueueTrait<std::shared_ptr<JSONContainer>> I;

  typedef I::Input Input;

  StoreJoinExec(std::shared_ptr<JoinManager> manager);

  void emptyBuffer(std::optional<Input>& buff);

  void finalize();

 private:
  std::shared_ptr<JoinManager> manager;
};

typedef IExecutor<StoreJoinExec, tasks::AsyncType::Synchronous> StoreJoin;

}  // namespace joda::queryexecution::pipeline::tasks::join

JODA_REGISTER_PIPELINE_TASK_IN_NS(StoreJoin, "StoreJoin",
                                  joda::queryexecution::pipeline::tasks::join)

#endif  // JODA_PIPELINE_STOREJOIN_H