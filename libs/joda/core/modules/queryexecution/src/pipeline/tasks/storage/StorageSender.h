#ifndef JODA_PIPELINE_STORAGESENDER_H
#define JODA_PIPELINE_STORAGESENDER_H

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/storage/JSONStorage.h>
namespace joda::queryexecution::pipeline::tasks::storage {

/**
 * Sends all containers existing in the storage (at time of task creation)
 * through the queue
 */
class StorageSenderExec {
 public:
  typedef queue::OutputQueueTrait<std::shared_ptr<JSONContainer>> O;

  typedef O::Output Output;

  StorageSenderExec(const std::string& storage);

  void fillBuffer(std::optional<Output>& obuff,
                  std::function<void(std::optional<Output>&)> sendPartial);

  void finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial){};

  bool finished() const;

  std::tuple<std::string, std::shared_ptr<JSONStorage>, unsigned long> getData()
      const;
  std::string toString() const;

 private:
  std::string storageName;
  std::shared_ptr<JSONStorage> storage = nullptr;
  unsigned long storageTmpId = 0;
  bool isfinished = false;
};

typedef OExecutor<StorageSenderExec, tasks::AsyncType::Synchronous>
    StorageSender;

}  // namespace joda::queryexecution::pipeline::tasks::storage

JODA_REGISTER_PIPELINE_TASK_IN_NS(
    StorageSender, "StorageSender",
    joda::queryexecution::pipeline::tasks::storage)

#endif  // JODA_PIPELINE_STORAGESENDER_H