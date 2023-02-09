#ifndef JODA_PIPELINE_LOADJOINFILES_H
#define JODA_PIPELINE_LOADJOINFILES_H

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>


#include <filesystem>

#include "../FileJoinManager.h"

namespace joda::queryexecution::pipeline::tasks::join {

/**
 * Loads all documents from the in-memory join and sends them in containers
 * through the queue
 */
class LoadJoinFilesExec {
 public:
  typedef queue::OutputQueueTrait<std::filesystem::path> O;

  typedef O::Output Output;

  LoadJoinFilesExec(const std::shared_ptr<FileJoinManager>& manager);

  void fillBuffer(std::optional<Output>& obuff,
                  std::function<void(std::optional<Output>&)> sendPartial);

  void finalize(std::optional<Output>& obuff){};

  bool finished() const;

 private:
  std::shared_ptr<FileJoinManager> manager;
  bool isfinished = false;
};

typedef OExecutor<LoadJoinFilesExec, tasks::AsyncType::SingleThreaded>
    LoadJoinFiles;

}  // namespace joda::queryexecution::pipeline::tasks::join

JODA_REGISTER_PIPELINE_TASK_IN_NS(LoadJoinFiles, "LoadJoinFiles",
                                  joda::queryexecution::pipeline::tasks::join)

#endif  // JODA_PIPELINE_LOADJOINFILES_H