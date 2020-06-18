//
// Created by Nico Schäfer on 11/27/17.
//

#include "../include/joda/query/values/INListProvider.h"
#include <rapidjson/fwd.h>
std::unique_ptr<joda::query::IValueProvider> joda::query::INListProvider::duplicate() const {
  return std::make_unique<joda::query::INListProvider>(duplicateParameters());
}

bool joda::query::INListProvider::isConst() const {
  return params[0]->isConst() && params[1]->isConst();
}


RJValue joda::query::INListProvider::getAtomValue(const RapidJsonDocument &json,
                                                  RJMemoryPoolAlloc &alloc) const {
  assert(isAtom() && "Did not check for atom before calling");
  RJMemoryPoolAlloc tmpAlloc;
  const RJValue *checkVal;
  RJValue tmpcheckVal;
  const RJValue *checkList;
  RJValue tmpcheckList;
  //Get Pointer to value
  if (params[0]->isAtom()) {
    tmpcheckVal = params[0]->getAtomValue(json, tmpAlloc);
    checkVal = &tmpcheckVal;
  } else {
    checkVal = params[0]->getValue(json, tmpAlloc);
  }
  if (checkVal == nullptr) return RJValue();

  //Get Pointer to List
  if (params[1]->isAtom()) {
    tmpcheckList = params[1]->getAtomValue(json, tmpAlloc);
    checkList = &tmpcheckList;
  } else {
    checkList = params[1]->getValue(json, tmpAlloc);
  }
  if (checkList == nullptr || checkList->IsNull()) return RJValue();

  RJValue val;
  //Check if checkVal in checkList
  if (checkList->IsArray()) {
    for (auto &v : checkList->GetArray()) {
      if (v == *checkVal) {
        val.SetBool(true);
        return val;
      }
    }
  }

  val.SetBool(false);
  return val;
}
const RJValue *joda::query::INListProvider::getValue(const RapidJsonDocument &json,
                                                     RJMemoryPoolAlloc &alloc) const {
  assert(!isAtom() && "Did not check for atom before calling");
  return nullptr;
}


joda::query::INListProvider::INListProvider(std::vector<std::unique_ptr<joda::query::IValueProvider>> &&parameters) : joda::query::IValueProvider(std::move(parameters)) {
  checkParamSize(2);
  checkParamType(1,IV_Array);
}

std::string joda::query::INListProvider::getName() const {
  return "IN";
}

joda::query::IValueType joda::query::INListProvider::getReturnType() const {
  return IV_Bool;
}
