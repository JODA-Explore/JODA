#ifndef JODA_PIPELINE_JSONStringifier_H
#define JODA_PIPELINE_JSONStringifier_H

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/container/JSONContainer.h>

#include <memory>

namespace joda::queryexecution::pipeline::tasks::store {

class JSONStringifierExec {
 public:
  typedef queue::InputQueueTrait<std::shared_ptr<JSONContainer>> I;
  typedef queue::OutputQueueTrait<std::vector<std::string>> O;

  typedef I::Input Input;
  typedef O::Output Output;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1; 

  static constexpr size_t OutBulk = 512; 

  JSONStringifierExec();

  void fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff,
                  std::function<void(std::optional<Output>&)> sendPartial);

  void finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial);

 private:
};

typedef IOExecutor<JSONStringifierExec, tasks::AsyncType::MultiThreaded>
    JSONStringifier;

}  // namespace joda::queryexecution::pipeline::tasks::store

JODA_REGISTER_PIPELINE_TASK_IN_NS(JSONStringifier, "JSONStringifier",
                                  joda::queryexecution::pipeline::tasks::store)

#endif  // JODA_PIPELINE_JSONStringifier_H