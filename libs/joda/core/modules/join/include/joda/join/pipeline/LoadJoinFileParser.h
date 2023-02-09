#ifndef JODA_PIPELINE_LOADJOINFILEPARSER_H
#define JODA_PIPELINE_LOADJOINFILEPARSER_H

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>

#include <filesystem>

#include "../FileJoinManager.h"

namespace joda::queryexecution::pipeline::tasks::join {

/**
 * Parses a join file and outputs finished JSONContainer
 */
class LoadJoinFileParserExec {
 public:
  typedef queue::InputQueueTrait<std::filesystem::path> I;
  typedef queue::OutputQueueTrait<std::shared_ptr<JSONContainer>> O;

  typedef I::Input Input;
  typedef O::Output Output;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1;

  LoadJoinFileParserExec(const std::shared_ptr<FileJoinManager>& manager);
  ~LoadJoinFileParserExec();

  void fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff,
                  std::function<void(std::optional<Output>&)> sendPartial);

  void finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial);

 private:
  std::shared_ptr<FileJoinManager> manager;
};

typedef IOExecutor<LoadJoinFileParserExec, tasks::AsyncType::MultiThreaded>
    LoadJoinFileParser;

}  // namespace joda::queryexecution::pipeline::tasks::join

JODA_REGISTER_PIPELINE_TASK_IN_NS(LoadJoinFileParser, "LoadJoinFileParser",
                                  joda::queryexecution::pipeline::tasks::join)

#endif  // JODA_PIPELINE_LOADJOINFILEPARSER_H