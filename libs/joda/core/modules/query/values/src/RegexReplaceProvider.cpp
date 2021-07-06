//
// Created by Nico Schäfer on 23/01/18.
//

#include "../include/joda/query/values/RegexReplaceProvider.h"
#include <glog/logging.h>

std::unique_ptr<joda::query::IValueProvider>
joda::query::RegexReplaceProvider::duplicate() const {
  return std::make_unique<joda::query::RegexReplaceProvider>(
      duplicateParameters());
}

bool joda::query::RegexReplaceProvider::isConst() const { return false; }

const RJValue* joda::query::RegexReplaceProvider::getValue(
    const RapidJsonDocument& /*json*/, RJMemoryPoolAlloc& /*alloc*/) const {
  DCHECK(!isAtom()) << "Did not check for atom before calling";
  return nullptr;
}

RJValue joda::query::RegexReplaceProvider::getAtomValue(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& alloc) const {
  DCHECK(isAtom()) << "Did not check for atom before calling";
  std::string toCheck;
  std::string replace;
  RJValue val;
  if (getParamString(toCheck, params[0], json) &&
      getParamString(replace, params[2], json)) {
    auto replaced = std::regex_replace(toCheck, regex, replace);
    val.SetString(replaced.c_str(), alloc);
  }
  return val;
}

joda::query::RegexReplaceProvider::RegexReplaceProvider(
    std::vector<std::unique_ptr<joda::query::IValueProvider>>&& parameters)
    : joda::query::IValueProvider(std::move(parameters)) {
  checkParamSize(3);
  checkParamType(0, IV_String);
  checkParamType(1, IV_String);
  checkParamType(2, IV_String);
  if (!this->params[1]->isConst()) {
    throw joda::query::WrongParameterException(
        getName() + ": Parameter 1 has to be constant");
  }
  if (params[1]->isConst()) {
    // Get Value
    RJMemoryPoolAlloc alloc;
    auto strVal = params[1]->getAtomValue(RapidJsonDocument(), alloc);
    std::string regstr = strVal.GetString();
    try {
      regex = std::regex(regstr, std::regex::optimize);
    } catch (const std::regex_error& e) {
      throw joda::query::WrongParameterException(
          getName() +
          ": The regex supplied in parameter 1 is invalid. Regex error: " +
          e.what());
    }
  } else {
    CHECK(false) << "Forgot to change logic of RegexExtractProvider";
  }
}

std::string joda::query::RegexReplaceProvider::getName() const {
  return "REGEX_REPLACE";
}

joda::query::IValueType joda::query::RegexReplaceProvider::getReturnType()
    const {
  return IV_String;
}
