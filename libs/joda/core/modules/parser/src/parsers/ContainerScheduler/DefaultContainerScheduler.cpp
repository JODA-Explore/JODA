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
std::unique_ptr<JSONContainer> joda::docparsing::DefaultContainerScheduler<true>::scheduleDocument(
    ContainerIdentifier /*id*/, std::unique_ptr<RJDocument>&& doc,
    std::unique_ptr<IOrigin>&& origin, size_t size) {
  std::unique_ptr<JSONContainer> tmpCont = nullptr;
  if (!(currentContainer->hasMetaSpace(size) ||
        currentContainer->size() == 0)) {
    currentContainer->metaFinalize();
    currentContainer->removeDocuments();
    DCHECK(currentContainer != nullptr);
    tmpCont = std::move(currentContainer);
    DCHECK(currentContainer == nullptr);
    currentContainer = createContainer(contSize);
    DCHECK(currentContainer != nullptr);
  }
  currentContainer->insertDoc(std::move(doc), std::move(origin));
  return tmpCont;
}

template <>
std::unique_ptr<JSONContainer> joda::docparsing::DefaultContainerScheduler<false>::scheduleDocument(
    ContainerIdentifier /*id*/, std::unique_ptr<RJDocument>&& doc,
    std::unique_ptr<IOrigin>&& origin, size_t size) {
  std::unique_ptr<JSONContainer> tmpCont = nullptr;
  DCHECK(currentContainer != nullptr);
  currentContainer->insertDoc(std::move(doc), std::move(origin));
  if (!(currentContainer->hasSpace(size) || currentContainer->size() == 0)) {
    currentContainer->finalize();
    tmpCont = std::move(currentContainer);

    DCHECK(currentContainer == nullptr);
    currentContainer = createContainer(contSize);
    DCHECK(currentContainer != nullptr);
  }
  
  return tmpCont;
}

template <bool meta>
joda::docparsing::DefaultContainerScheduler<meta>::DefaultContainerScheduler(size_t contSize)
    : contSize(contSize) {
  currentContainer = createContainer(contSize);
  DCHECK(currentContainer != nullptr);
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
std::vector<std::unique_ptr<JSONContainer>> joda::docparsing::DefaultContainerScheduler<true>::finalize() {
  DCHECK(currentContainer != nullptr);
  if (currentContainer->size() > 0) {
    currentContainer->metaFinalize();
    currentContainer->removeDocuments();
    std::vector<std::unique_ptr<JSONContainer>> ret;
    ret.emplace_back(std::move(currentContainer));
    return ret;
  }
  return {};
}

template <>
std::vector<std::unique_ptr<JSONContainer>> joda::docparsing::DefaultContainerScheduler<false>::finalize() {
  DCHECK(currentContainer != nullptr);
  if (currentContainer->size() > 0) {
    currentContainer->finalize();
    if (!config::storeJson) {  // Precaution, normally Meta should be true in
                               // this case
      currentContainer->removeDocuments();
    }

    std::vector<std::unique_ptr<JSONContainer>> ret;
    ret.emplace_back(std::move(currentContainer));
    return ret;
  }
  return {};
}


template class joda::docparsing::DefaultContainerScheduler<true>;
template class joda::docparsing::DefaultContainerScheduler<false>;