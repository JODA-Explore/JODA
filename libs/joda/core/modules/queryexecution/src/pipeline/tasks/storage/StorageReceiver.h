#ifndef JODA_PIPELINE_STORAGERECEIVER_H
#define JODA_PIPELINE_STORAGERECEIVER_H

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/storage/JSONStorage.h>

namespace joda::queryexecution::pipeline::tasks::storage {

/**
 * Stores all containers passed by queue in the storage
 */
class StorageReceiverExec {
 public:
  typedef queue::InputQueueTrait<std::shared_ptr<JSONContainer>> I;

  typedef I::Input Input;

  StorageReceiverExec(const std::string& storage);
  StorageReceiverExec(unsigned long storageId);

  void emptyBuffer(std::optional<Input>& buff);

  void finalize();

  std::tuple<std::string, std::shared_ptr<JSONStorage>, unsigned long> getData()
      const;
  std::string toString() const;

 private:
  std::string storageName;
  std::shared_ptr<JSONStorage> storage = nullptr;
  unsigned long storageTmpId = 0;
};

typedef IExecutor<StorageReceiverExec, tasks::AsyncType::Synchronous>
    StorageReceiver;

}  // namespace joda::queryexecution::pipeline::tasks::storage

JODA_REGISTER_PIPELINE_TASK_IN_NS(
    StorageReceiver, "StorageReceiver",
    joda::queryexecution::pipeline::tasks::storage)

#endif  // JODA_PIPELINE_STORAGERECEIVER_H