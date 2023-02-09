#include "StorageReceiver.h"

#include <joda/storage/collection/StorageCollection.h>
void joda::queryexecution::pipeline::tasks::storage::StorageReceiverExec::
    emptyBuffer(std::optional<Input>& buff) {
  if (storage == nullptr) {
    if (storageName.empty()) {
      storage = StorageCollection::getInstance().getStorage(storageTmpId);
    } else {
      storage = StorageCollection::getInstance().getOrAddStorage(storageName);
    }
    DCHECK_NOTNULL(this->storage);
  }

  if (buff.has_value()) {
    auto cont = std::move(buff.value());
    buff.reset();
    if (cont == nullptr) {
      return;
    }
    // Store container
    storage->insertDocuments(std::move(cont));
  }
}

void joda::queryexecution::pipeline::tasks::storage::StorageReceiverExec::
    finalize() {
}

joda::queryexecution::pipeline::tasks::storage::StorageReceiverExec::
    StorageReceiverExec(const std::string& storage)
    : storageName(storage) {}

joda::queryexecution::pipeline::tasks::storage::StorageReceiverExec::
    StorageReceiverExec(unsigned long storageId)
    : storageName(""), storageTmpId(storageId) {}


std::tuple<std::string, std::shared_ptr<JSONStorage>, unsigned long>
joda::queryexecution::pipeline::tasks::storage::StorageReceiverExec::getData()
    const {
  return std::make_tuple(storageName, storage, storageTmpId);
}

std::string joda::queryexecution::pipeline::tasks::storage::StorageReceiverExec::toString() const{
  if(!storageName.empty()){
    return "Storage: " + storageName;
  }
  return "Temporary Storage: " + std::to_string(storageTmpId);
}