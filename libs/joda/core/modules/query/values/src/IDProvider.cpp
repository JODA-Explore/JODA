//
// Created by Nico Schäfer on 12/7/17.
//

#include "../include/joda/query/values/IDProvider.h"

std::unique_ptr<joda::query::IValueProvider> joda::query::IDProvider::duplicate() const {
  return std::make_unique<joda::query::IDProvider>(duplicateParameters());
}

bool joda::query::IDProvider::isConst() const {
  return false;
}

RJValue joda::query::IDProvider::getAtomValue(const RapidJsonDocument &json,
                                              RJMemoryPoolAlloc &alloc) const {
  assert(isAtom() && "Did not check for atom before calling");
  RJValue val;
  val.SetInt64(json.getId());
  return val;
}
const RJValue *joda::query::IDProvider::getValue(const RapidJsonDocument &json,
                                                 RJMemoryPoolAlloc &alloc) const {
  assert(!isAtom() && "Did not check for atom before calling");
  return nullptr;
}


std::string joda::query::IDProvider::getName() const {
  return "ID";
}

joda::query::IValueType joda::query::IDProvider::getReturnType() const {
  return IV_Number;
}

joda::query::IDProvider::IDProvider(std::vector<std::unique_ptr<IValueProvider>> &&parameters) : IValueProvider(std::move(parameters)) {
  checkParamSize(0);
}
