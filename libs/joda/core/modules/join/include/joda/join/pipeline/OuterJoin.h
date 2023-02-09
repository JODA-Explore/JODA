#ifndef JODA_PIPELINE_OUTER_JOIN_H
#define JODA_PIPELINE_OUTER_JOIN_H

#include <joda/container/JSONContainer.h>
#include <joda/join/relation/ContainerJoinExecutor.h>
#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>

namespace joda::queryexecution::pipeline::tasks::join {

/**
 * Performs the join operation for the outer join partner
 */
class OuterJoinExec {
 public:
  typedef queue::InputQueueTrait<std::shared_ptr<JSONContainer>> I;
  typedef queue::OutputQueueTrait<std::shared_ptr<JSONContainer>> O;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1;

  OuterJoinExec(std::shared_ptr<joda::join::ContainerJoinExecutor> joiner);

  void fillBuffer(std::optional<I::Input>& ibuff, std::optional<O::Output>& obuff,
                  std::function<void(std::optional<O::Output>&)> sendPartial);

  void finalize(std::optional<O::Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial);

  std::string toString() const;

 private:
  std::shared_ptr<joda::join::ContainerJoinExecutor> joiner;
};

typedef IOExecutor<OuterJoinExec, tasks::AsyncType::MultiThreaded> OuterJoin;

}  // namespace joda::queryexecution::pipeline::tasks::join

JODA_REGISTER_PIPELINE_TASK_IN_NS(OuterJoin, "OuterJoin",
                                  joda::queryexecution::pipeline::tasks::join)

#endif  // JODA_PIPELINE_OUTER_JOIN_H