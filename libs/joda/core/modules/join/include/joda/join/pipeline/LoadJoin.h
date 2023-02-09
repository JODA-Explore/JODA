#ifndef JODA_PIPELINE_LOADJOIN_H
#define JODA_PIPELINE_LOADJOIN_H

#include <joda/storage/JSONStorage.h>
#include "../MemoryJoinManager.h"


#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
namespace joda::queryexecution::pipeline::tasks::join {

/**
 * Loads all documents from the in-memory join and sends them in containers through the queue
 */
class LoadJoinExec {
 public:
  typedef queue::OutputQueueTrait<std::shared_ptr<JSONContainer>> O;

  typedef O::Output Output;


  LoadJoinExec(const std::shared_ptr<MemoryJoinManager>& manager);

  void fillBuffer(std::optional<Output>& obuff, std::function<void(std::optional<Output>&)> sendPartial);

  void finalize(std::optional<Output>& obuff){};

  bool finished() const;

 private:
  std::shared_ptr<MemoryJoinManager> manager;
  bool isfinished = false;
};

typedef OExecutor<LoadJoinExec, tasks::AsyncType::SingleThreaded> LoadJoin;


}  // namespace joda::queryexecution::pipeline::tasks::join

JODA_REGISTER_PIPELINE_TASK_IN_NS(LoadJoin, "LoadJoin",
                                  joda::queryexecution::pipeline::tasks::join)

#endif  // JODA_PIPELINE_LOADJOIN_H