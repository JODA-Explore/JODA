//
// Created by Nico Schäfer on 23/01/18.
//

#ifndef JODA_REGEXPROVIDER_H
#define JODA_REGEXPROVIDER_H

#include <regex>

#include "IValueProvider.h"

namespace joda::query {
/**
 * REGEX(), checks if a given string matches a regex string
 */
class RegexProvider : public joda::query::IValueProvider {
 public:
  RegexProvider(std::vector<std::unique_ptr<IValueProvider>> &&parameters);

  std::unique_ptr<IValueProvider> duplicate() const override;

  bool isConst() const override;

  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;
  std::string getName() const override;
  IValueType getReturnType() const override;

  CREATE_FACTORY(RegexProvider)

 private:
  std::regex regex;
};
}  // namespace joda::query

#endif  // JODA_REGEXPROVIDER_H
