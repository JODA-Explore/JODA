#ifndef JODA_PIPELINE_INNERJOIN_H
#define JODA_PIPELINE_INNERJOIN_H

#include <joda/join/relation/ContainerJoinExecutor.h>
#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/storage/JSONStorage.h>
namespace joda::queryexecution::pipeline::tasks::join {

/**
 * Handles the inner join of the two relations
 */
class InnerJoinExec {
 public:
  typedef queue::InputQueueTrait<std::shared_ptr<JSONContainer>> I;

  typedef I::Input Input;

  InnerJoinExec(std::shared_ptr<joda::join::ContainerJoinExecutor> joiner);

  void emptyBuffer(std::optional<Input>& buff);

  void finalize();

  std::string toString() const;

 private:
  std::shared_ptr<joda::join::ContainerJoinExecutor> joiner;
};

typedef IExecutor<InnerJoinExec, tasks::AsyncType::Synchronous> InnerJoin;

}  // namespace joda::queryexecution::pipeline::tasks::join

JODA_REGISTER_PIPELINE_TASK_IN_NS(InnerJoin, "InnerJoin",
                                  joda::queryexecution::pipeline::tasks::join)

#endif  // JODA_PIPELINE_INNERJOIN_H