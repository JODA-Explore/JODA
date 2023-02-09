

#include "../include/joda/query/values/RegexExtractFirstProvider.h"
#include <glog/logging.h>

std::unique_ptr<joda::query::IValueProvider>
joda::query::RegexExtractFirstProvider::duplicate() const {
  return std::make_unique<joda::query::RegexExtractFirstProvider>(
      duplicateParameters());
}

bool joda::query::RegexExtractFirstProvider::isConst() const { return false; }

RJValue joda::query::RegexExtractFirstProvider::getAtomValue(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& alloc) const {
  DCHECK(isAtom()) << "Did not check for atom before calling";
  std::string toCheck;
  RJValue val;
  if (getParamString(toCheck, params[0], json)) {
    auto words_begin =
        std::sregex_iterator(toCheck.begin(), toCheck.end(), regex);
    auto words_end = std::sregex_iterator();
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
      std::smatch match = *i;
      if (match.size() > 1) {
        std::string match_str = match.str(1);
        RJValue strVal;
        strVal.SetString(match_str.c_str(), alloc);
        return strVal;
      }
    }
  }
  return RJValue();
}

bool joda::query::RegexExtractFirstProvider::isAtom() const { return true; }

const RJValue* joda::query::RegexExtractFirstProvider::getValue(
    const RapidJsonDocument& /*json*/, RJMemoryPoolAlloc& /*alloc*/) const {
  DCHECK(!isAtom()) << "Did not check for atom before calling";
  return nullptr;
}

joda::query::RegexExtractFirstProvider::RegexExtractFirstProvider(
    std::vector<std::unique_ptr<joda::query::IValueProvider>>&& parameters)
    : joda::query::IValueProvider(std::move(parameters)) {
  checkParamSize(2);
  checkParamType(0, IV_String);
  checkParamType(1, IV_String);
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
    CHECK(false) << "Forgot to change logic of RegexExtractFirstProvider";
  }
}

std::string joda::query::RegexExtractFirstProvider::getName() const {
  return "REGEX_EXTRACT_FIRST";
}

joda::query::IValueType joda::query::RegexExtractFirstProvider::getReturnType()
    const {
  return IV_String;
}
