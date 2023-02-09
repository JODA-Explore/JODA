//
// Created by Nico on 20/03/2019.
//

#include "../include/joda/export/StorageExport.h"
#include <joda/storage/collection/StorageCollection.h>

#include <utility>


const std::string StorageExport::getTimerName() { return "Store"; }

StorageExport::StorageExport(std::shared_ptr<JSONStorage> store)
    : store(std::move(store)) {}


unsigned long StorageExport::getTemporaryResultID() const {
  return StorageCollection::getInstance().addTemporaryStorage(store);
}

const std::string StorageExport::toString() {
  return "Store in collection: " + getStorageName();
}

const std::string StorageExport::toQueryString() {
  return "STORE " + getStorageName();
}

std::string StorageExport::getStorageName() const { return store->getName(); }

const std::shared_ptr<JSONStorage>& StorageExport::getStore() const {
  return store;
}

 StorageExport::PipelineTaskPtr StorageExport::getTask() const {
   return nullptr;

}
