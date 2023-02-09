#ifndef JODA_PIPELINE_STORAGEBUFFER_H
#define JODA_PIPELINE_STORAGEBUFFER_H

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/storage/JSONStorage.h>

namespace joda::queryexecution::pipeline::tasks::storage {

/**
 * Stores all containers in the given Storage and passes them on as reference
 * Additionally all already included containers are also sent.
 */
class StorageBufferExec {
 public:
  typedef queue::InputQueueTrait<std::shared_ptr<JSONContainer>> I;
  typedef queue::OutputQueueTrait<std::shared_ptr<JSONContainer>> O;

  typedef I::Input Input;
  typedef O::Output Output;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1;

  StorageBufferExec(const std::string& storage);

  void fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff,
                  std::function<void(std::optional<Output>&)> sendPartial);

  void finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial);

  std::tuple<std::string, std::shared_ptr<JSONStorage>, unsigned long> getData()
      const;
  std::string toString() const;

 private:
  std::string storageName;
  std::shared_ptr<JSONStorage> storage = nullptr;
  unsigned long storageTmpId = 0;

  bool copied = false;
};

typedef IOExecutor<StorageBufferExec, tasks::AsyncType::Synchronous>
    StorageBuffer;

}  // namespace joda::queryexecution::pipeline::tasks::storage

JODA_REGISTER_PIPELINE_TASK_IN_NS(
    StorageBuffer, "StorageBuffer",
    joda::queryexecution::pipeline::tasks::storage)

#endif  // JODA_PIPELINE_STORAGEBUFFER_H