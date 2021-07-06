//
// Created by Nico Schäfer on 23/01/18.
//

#ifndef JODA_REGEXREPLACEPROVIDER_H
#define JODA_REGEXREPLACEPROVIDER_H

#include <regex>

#include "IValueProvider.h"

namespace joda::query {
/**
 * REGEX_REPLACE(), replaces a matching part of a string with another string
 */
class RegexReplaceProvider : public joda::query::IValueProvider {
 public:
  RegexReplaceProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters);
  std::unique_ptr<IValueProvider> duplicate() const override;
  bool isConst() const override;

  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;

  std::string getName() const override;
  IValueType getReturnType() const override;

  CREATE_FACTORY(RegexReplaceProvider)
 private:
  std::regex regex;
};
}  // namespace joda::query

#endif  // JODA_REGEXREPLACEPROVIDER_H
