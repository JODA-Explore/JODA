//
// Created by Nico on 13/05/2019.
//

#include "DefaultContainerScheduler.h"

#include <joda/config/config.h>
#include <joda/storage/JSONStorage.h>

template <bool meta>
std::unique_ptr<JSONContainer>
joda::docparsing::DefaultContainerScheduler<meta>::createContainer(
    size_t contSize) const {
  if (contSize == 0) {
    return std::make_unique<JSONContainer>();  //& Create new one
  }
  {
    return std::make_unique<JSONContainer>(contSize);  //& Create new one
  }
}

template <>
void joda::docparsing::DefaultContainerScheduler<true>::scheduleDocument(
    ContainerIdentifier /*id*/, std::unique_ptr<RJDocument>&& doc,
    std::unique_ptr<IOrigin>&& origin, size_t size) {
  if (!(currentContainer->hasMetaSpace(size) ||
        currentContainer->size() == 0)) {
    currentContainer->metaFinalize();
    currentContainer->removeDocuments();
    DCHECK(currentContainer != nullptr);
    queue->send(std::move(currentContainer));  // Enqueue
    DCHECK(currentContainer == nullptr);
    currentContainer = createContainer(contSize);
    DCHECK(currentContainer != nullptr);
  }
  currentContainer->insertDoc(std::move(doc), std::move(origin));
}

template <>
void joda::docparsing::DefaultContainerScheduler<false>::scheduleDocument(
    ContainerIdentifier /*id*/, std::unique_ptr<RJDocument>&& doc,
    std::unique_ptr<IOrigin>&& origin, size_t size) {
  DCHECK(currentContainer != nullptr);
  if (!(currentContainer->hasSpace(size) || currentContainer->size() == 0)) {
    currentContainer->finalize();
    queue->send(std::move(currentContainer));  // Enqueue
    DCHECK(currentContainer == nullptr);
    currentContainer = createContainer(contSize);
    DCHECK(currentContainer != nullptr);
  }
  currentContainer->insertDoc(std::move(doc), std::move(origin));
}

template <bool meta>
joda::docparsing::DefaultContainerScheduler<meta>::DefaultContainerScheduler(
    JsonContainerQueue::queue_t* queue, size_t contSize)
    : queue(queue), contSize(contSize) {
  currentContainer = createContainer(contSize);
}

template <bool meta>
typename joda::docparsing::DefaultContainerScheduler<meta>::ContainerIdentifier
joda::docparsing::DefaultContainerScheduler<meta>::getContainerForDoc(
    std::string& /*raw*/) {
  return 0;
}

template <bool meta>
typename joda::docparsing::DefaultContainerScheduler<meta>::ContainerIdentifier
joda::docparsing::DefaultContainerScheduler<meta>::getContainerForDoc(
    const RJDocument& /*doc*/) {
  return 0;
}

template <bool meta>
typename joda::docparsing::DefaultContainerScheduler<meta>::ContainerIdentifier
joda::docparsing::DefaultContainerScheduler<meta>::getContainerForDoc(
    rapidjson::IStreamWrapper& /*stream*/) {
  return 0;
}

template <bool meta>
std::unique_ptr<RJDocument>
joda::docparsing::DefaultContainerScheduler<meta>::getNewDoc(
    ContainerIdentifier /*id*/) {
  return std::make_unique<RJDocument>(currentContainer->getAlloc());
}

template <>
void joda::docparsing::DefaultContainerScheduler<true>::finalize() {
  DCHECK(currentContainer != nullptr);
  if (currentContainer->size() > 0) {
    currentContainer->metaFinalize();
    currentContainer->removeDocuments();
    queue->send(std::move(currentContainer));  // Enqueue
    DCHECK(currentContainer == nullptr);
  }
  queue->producerFinished();
}

template <>
void joda::docparsing::DefaultContainerScheduler<false>::finalize() {
  DCHECK(currentContainer != nullptr);
  if (currentContainer->size() > 0) {
    currentContainer->finalize();
    if (!config::storeJson) {  // Precaution, normally Meta should be true in
                               // this case
      currentContainer->removeDocuments();
    }
    queue->send(std::move(currentContainer));  // Enqueue
    DCHECK(currentContainer == nullptr);
  }
  queue->producerFinished();
}

template class joda::docparsing::DefaultContainerScheduler<true>;
template class joda::docparsing::DefaultContainerScheduler<false>;