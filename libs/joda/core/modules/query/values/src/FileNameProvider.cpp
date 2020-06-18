//
// Created by Nico Schäfer on 12/7/17.
//

#include <joda/document/FileOrigin.h>
#include "../include/joda/query/values/FileNameProvider.h"
#include <joda/misc/FileNameRepo.h>
std::unique_ptr<joda::query::IValueProvider> joda::query::FileNameProvider::duplicate() const {
  return std::make_unique<joda::query::FileNameProvider>(duplicateParameters());
}

bool joda::query::FileNameProvider::isConst() const {
  return false;
}

RJValue joda::query::FileNameProvider::getAtomValue(const RapidJsonDocument &json,
                                                    RJMemoryPoolAlloc &alloc) const {
  assert(isAtom() && "Did not check for atom before calling");
  RJValue val;
  auto* tmp = json.getOrigin();
  auto* fileOrig = dynamic_cast<const FileOrigin*>(tmp);
  if(fileOrig != nullptr)
    val.SetString(g_FileNameRepoInstance.getFile(fileOrig->getFile()).c_str(), alloc);
  return val;
}
const RJValue *joda::query::FileNameProvider::getValue(const RapidJsonDocument &json,
                                                       RJMemoryPoolAlloc &alloc) const {
  assert(!isAtom() && "Did not check for atom before calling");
  return nullptr;
}

std::string joda::query::FileNameProvider::getName() const {
  return "FILENAME";
}

joda::query::IValueType joda::query::FileNameProvider::getReturnType() const {
  return IV_String;
}

joda::query::FileNameProvider::FileNameProvider(std::vector<std::unique_ptr<IValueProvider>> &&parameters) : IValueProvider(
    std::move(parameters)) {
checkParamSize(0);
}
