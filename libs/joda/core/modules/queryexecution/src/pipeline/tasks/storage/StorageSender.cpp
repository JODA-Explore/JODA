#include "StorageSender.h"

#include <joda/storage/collection/StorageCollection.h>
void joda::queryexecution::pipeline::tasks::storage::StorageSenderExec::
    fillBuffer(std::optional<Output>& obuff,
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

  storage->forAll([&](const auto& cont) {
    obuff = cont;
    sendPartial(obuff);
  });
  isfinished = true;
}

bool joda::queryexecution::pipeline::tasks::storage::StorageSenderExec::
    finished() const {
  return isfinished;
}

joda::queryexecution::pipeline::tasks::storage::StorageSenderExec::
    StorageSenderExec(const std::string& storage)
    : storageName(storage) {}

std::tuple<std::string, std::shared_ptr<JSONStorage>, unsigned long>
joda::queryexecution::pipeline::tasks::storage::StorageSenderExec::getData()
    const {
  return std::make_tuple(storageName, storage, storageTmpId);
}

std::string
joda::queryexecution::pipeline::tasks::storage::StorageSenderExec::toString()
    const {
  if (!storageName.empty()) {
    return "Storage: " + storageName;
  }
  return "Temporary Storage: " + std::to_string(storageTmpId);
}