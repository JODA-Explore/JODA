//
// Created by Nico Schäfer
//

#include "../include/joda/query/values/SubStringProvider.h"

#include <glog/logging.h>

std::unique_ptr<joda::query::IValueProvider>
joda::query::SubStringProvider::duplicate() const {
  return std::make_unique<joda::query::SubStringProvider>(
      duplicateParameters());
}

bool joda::query::SubStringProvider::isConst() const {
  bool thirdConst = true;
  if (params.size() == 3) {
    thirdConst &= params[2]->isConst();
  }
  return params[0]->isConst() && params[1]->isConst() && thirdConst;
}

RJValue joda::query::SubStringProvider::getAtomValue(
    const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) const {
  DCHECK(isAtom()) << "Did not check for atom before calling";
  std::string toCheck;
  RJValue val;
  if (getParamString(toCheck, params[0], json)) {
    auto begin = (u_int64_t)0;
    if (params[1]->isAtom()) {
      auto first = params[1]->getAtomValue(json, alloc);
      if (first.IsUint64()) {
        begin = first.GetUint64();
      }
      begin = std::min(
          begin,
          toCheck.size());  // Never larger than string, or exception is thrown
    }
    if (params.size() == 2) {
      val = RJValue(utf8_substr(toCheck,begin).c_str(), alloc);
    } else if (params.size() == 3) {
      auto len = (u_int64_t)toCheck.size();
      if (params[2]->isAtom()) {
        auto second = params[2]->getAtomValue(json, alloc);
        if (second.IsUint64()) {
          len = second.GetUint64();
        }
      }
      val = RJValue(utf8_substr(toCheck,begin,len).c_str(), alloc);
    }
  }
  return std::move(val);
}

bool joda::query::SubStringProvider::isAtom() const { return true; }

const RJValue *joda::query::SubStringProvider::getValue(
    const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) const {
  DCHECK(!isAtom()) << "Did not check for atom before calling";
  return nullptr;
}

joda::query::SubStringProvider::SubStringProvider(
    std::vector<std::unique_ptr<joda::query::IValueProvider>> &&parameters)
    : joda::query::IValueProvider(std::move(parameters)) {
  if (params.size() < 2 || params.size() > 3) {
    throw WrongParameterCountException(params.size(), 2, getName());
  }
  checkParamType(0, IV_String);
  checkParamType(1, IV_Number);
  if (params.size() == 3) {
    checkParamType(2, IV_Number);
  }
}

std::string joda::query::SubStringProvider::getName() const { return "SUBSTR"; }

joda::query::IValueType joda::query::SubStringProvider::getReturnType() const {
  return IV_String;
}
