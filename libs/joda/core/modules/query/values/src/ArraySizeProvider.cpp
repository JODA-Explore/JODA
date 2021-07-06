//
// Created by Nico Schäfer on 11/27/17.
//

#include "../include/joda/query/values/ArraySizeProvider.h"
#include <glog/logging.h>
#include <rapidjson/fwd.h>
#include <tao/pegtl/parse_error.hpp>

std::unique_ptr<joda::query::IValueProvider>
joda::query::ArraySizeProvider::duplicate() const {
  return std::make_unique<ArraySizeProvider>(duplicateParameters());
}

bool joda::query::ArraySizeProvider::isConst() const { return false; }

RJValue joda::query::ArraySizeProvider::getAtomValue(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& alloc) const {
  DCHECK(isAtom()) << "Did not check for atom before calling";
  if (!params[0]->isAtom()) {
    RJValue val;
    auto* arr = params[0]->getValue(json, alloc);
    if (arr != nullptr && arr->IsArray()) {
      val.SetInt64(arr->Size());
    }
    return val;
  }
  RJValue val;
  auto arr = params[0]->getAtomValue(json, alloc);
  if (arr.IsArray()) {
    val.SetInt64(arr.Size());
  }
  return val;
}
const RJValue* joda::query::ArraySizeProvider::getValue(
    const RapidJsonDocument& /*json*/, RJMemoryPoolAlloc& /*alloc*/) const {
  DCHECK(!isAtom()) << "Did not check for atom before calling";
  return nullptr;
}

joda::query::ArraySizeProvider::ArraySizeProvider(
    std::vector<std::unique_ptr<IValueProvider>>&& parameters)
    : IValueProvider(std::move(parameters)) {
  checkParamSize(1);
  checkParamType(0, IV_Array);
}

joda::query::ArraySizeProvider::ArraySizeProvider() {
  throw joda::query::WrongParameterCountException(0, 1, getName());
}

joda::query::IValueType joda::query::ArraySizeProvider::getReturnType() const {
  return IV_Number;
}

std::string joda::query::ArraySizeProvider::getName() const { return "SIZE"; }
