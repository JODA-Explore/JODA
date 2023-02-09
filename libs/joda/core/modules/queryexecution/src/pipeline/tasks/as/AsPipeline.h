#ifndef JODA_PIPELINE_ASPIPELINE_H
#define JODA_PIPELINE_ASPIPELINE_H

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/storage/JSONStorage.h>

#include "../choose/ChoosePipeline.h"

namespace joda::queryexecution::pipeline::tasks::as {

/**
 * Performs the AS operation on one container
 */
class AsExec {
 public:
  typedef queue::InputQueueTrait<choose::FilteredJSONContainer> I;
  typedef queue::OutputQueueTrait<std::shared_ptr<JSONContainer>> O;

  typedef I::Input Input;
  typedef O::Output Output;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1;

  AsExec(const std::shared_ptr<const query::Query>& query);

  void fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff,
                  std::function<void(std::optional<Output>&)> sendPartial);

  void finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial);

  bool hasToProject() const;

  /**
   * @brief Projects a single container
   *
   * @param input The input container
   * @return std::optional<O::Output> The projected container
   */
  std::optional<O::Output> handleOne(I::Input& input);

  std::shared_ptr<const query::Query> getData() const;
  std::string toString() const;

 private:
  std::shared_ptr<const query::Query> query;
  std::vector<std::string> asAttributes;
  bool projectionRequired = true;

  std::vector<std::unique_ptr<RJDocument>> defaultProject(
      JSONContainer& cont, const DocIndex& ids, RJMemoryPoolAlloc& alloc) const;
};

typedef IOExecutor<AsExec, tasks::AsyncType::MultiThreaded> AsPipeline;

}  // namespace joda::queryexecution::pipeline::tasks::as

JODA_REGISTER_PIPELINE_TASK_IN_NS(AsPipeline, "AsPipeline",
                                  joda::queryexecution::pipeline::tasks::as)

#endif  // JODA_PIPELINE_ASPIPELINE_H