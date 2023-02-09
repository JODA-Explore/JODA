#ifndef JODA_PIPELINE_CHOOSEAS_PIPELINE_H
#define JODA_PIPELINE_CHOOSEAS_PIPELINE_H

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/storage/JSONStorage.h>

#include <joda/queryexecution/IQueryExecutor.h>
#include "../../tasks/as/AsPipeline.h"
#include "../../tasks/choose/ChoosePipeline.h"

namespace joda::queryexecution::pipeline::tasks::optimization {

/**
 * Performs the CHOOSE and AS operation on one container
 */
class ChooseAsExec {
 public:
  using I = queue::InputQueueTrait<std::shared_ptr<JSONContainer>>;
  using O = queue::OutputQueueTrait<std::shared_ptr<JSONContainer>>;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1;

  ChooseAsExec(const std::vector<std::unique_ptr<IQueryExecutor>>& execs,
               const std::shared_ptr<const query::Query>& query);
  ChooseAsExec(const ChooseAsExec& other);
  ChooseAsExec(ChooseAsExec&& other) = delete;
  ~ChooseAsExec() = default;

  void fillBuffer(std::optional<I::Input>& ibuff, std::optional<O::Output>& obuff,
                  std::function<void(std::optional<O::Output>&)> sendPartial);

  void finalize(std::optional<O::Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial);

  /**
   * @brief Filters a single container
   *
   * @param input The input container
   * @return std::vector<O::Output> The filtered container
   */
  std::optional<O::Output> handleOne(I::Input& input);

  std::string toString() const;

 private:
  choose::ChooseExec choose;
  as::AsExec as;
};

using ChooseAsPipeline = IOExecutor<ChooseAsExec, tasks::AsyncType::MultiThreaded>;

}  // namespace joda::queryexecution::pipeline::tasks::optimization

JODA_REGISTER_PIPELINE_TASK_IN_NS(
    ChooseAsPipeline, "ChooseAsPipeline",
    joda::queryexecution::pipeline::tasks::optimization)

#endif  // JODA_PIPELINE_CHOOSEAS_PIPELINE_H