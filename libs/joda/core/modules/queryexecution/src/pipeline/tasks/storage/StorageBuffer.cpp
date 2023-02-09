#include "StorageBuffer.h"

#include <joda/storage/collection/StorageCollection.h>

void joda::queryexecution::pipeline::tasks::storage::StorageBufferExec::
    fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff,
               std::function<void(std::optional<Output>&)> sendPartial) {
  if (storage == nullptr) {
    if (storageName.empty()) {
      storage = std::make_shared<JSONStorage>("");
      storageTmpId =
          StorageCollection::getInstance().addTemporaryStorage(storage);
    } else {
      storage = StorageCollection::getInstance().getOrAddStorage(storageName);
    }
    DCHECK_NOTNULL(this->storage);
  }

  if (!copied) {
    auto conts = storage->copyContainers();
    copied = true;
    for (auto&& cont : conts) {
      obuff= std::move(cont);
        sendPartial(obuff);
      
    }
  }

  if (ibuff.has_value()) {
    auto cont = std::move(ibuff.value());
    ibuff.reset();
    if (cont == nullptr) {
      return;
    }
    auto copy = cont;
    // Store container
    storage->insertDocuments(std::move(cont));
    // Move pointer to outqueue
    obuff = std::move(copy);

  }
}

void joda::queryexecution::pipeline::tasks::storage::StorageBufferExec::
    finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial) {
}

joda::queryexecution::pipeline::tasks::storage::StorageBufferExec::
    StorageBufferExec(const std::string& storage)
    : storageName(storage) {}


std::tuple<std::string, std::shared_ptr<JSONStorage>, unsigned long>
joda::queryexecution::pipeline::tasks::storage::StorageBufferExec::getData()
    const {
  return std::make_tuple(storageName, storage, storageTmpId);
}

std::string joda::queryexecution::pipeline::tasks::storage::StorageBufferExec::toString() const{
  if(!storageName.empty()){
    return "Storage: " + storageName;
  }
  return "Temporary Storage: " + std::to_string(storageTmpId);
}