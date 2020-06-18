//
// Created by Nico Schäfer on 12/7/17.
//

#include "../include/joda/query/values/TypeProvider.h"

std::unique_ptr<joda::query::IValueProvider> joda::query::TypeProvider::duplicate() const {
  return std::make_unique<TypeProvider>(duplicateParameters());
}

bool joda::query::TypeProvider::isConst() const {
  return false;
}

RJValue joda::query::TypeProvider::getAtomValue(const RapidJsonDocument &json,
                                                RJMemoryPoolAlloc &alloc) const {
  assert(isAtom() && "Did not check for atom before calling");
  RJValue val;
  if (params[0]->isAny()) {
    auto *target = params[0]->getValue(json, alloc);
    if (target != nullptr) {
      std::string str;
      if (target->IsObject()) str = "OBJECT";
      else if (target->IsNumber()) str = "NUMBER";
      else if (target->IsBool()) str = "BOOL";
      else if (target->IsString()) str = "STRING";
      else if (target->IsArray()) str = "ARRAY";
      else if (target->IsNull()) str = "NULL";
      val.SetString(str.c_str(), alloc);
    }
  } else {
    std::string str;
    if (params[0]->isNumber()) str = "NUMBER";
    else if (params[0]->isBool()) str = "BOOL";
    else if (params[0]->isString()) str = "STRING";
    else if (params[0]->isArray()) str = "ARRAY";
    else if (params[0]->isObject()) str = "OBJECT";
    else if (params[0]->isNull()) str = "NULL";
    else
      assert(false && "Unhandled type");
    val.SetString(str.c_str(), alloc);
  }
  return val;
}
const RJValue *joda::query::TypeProvider::getValue(const RapidJsonDocument &json,
                                                   RJMemoryPoolAlloc &alloc) const {
  assert(!isAtom() && "Did not check for atom before calling");
  return nullptr;
}


std::string joda::query::TypeProvider::getName() const {
  return "TYPE";
}

joda::query::IValueType joda::query::TypeProvider::getReturnType() const {
  return IV_String;
}

joda::query::TypeProvider::TypeProvider(std::vector<std::unique_ptr<IValueProvider>> &&parameters) : IValueProvider(std::move(parameters)) {
  checkParamSize(1);
}
