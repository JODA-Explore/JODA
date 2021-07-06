//
// Created by Nico Schäfer on 23/01/18.
//

#ifndef JODA_REGEXEXTRACTPROVIDER_H
#define JODA_REGEXEXTRACTPROVIDER_H

#include <regex>

#include "IValueProvider.h"
namespace joda::query {
/**
 * REGEX_EXTRACT(), extracts matching groups from a string, based on a regex
 */
class RegexExtractProvider : public joda::query::IValueProvider {
 public:
  explicit RegexExtractProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters);
  std::unique_ptr<IValueProvider> duplicate() const override;
  bool isConst() const override;

  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;

  bool isAtom() const override;
  std::string getName() const override;
  IValueType getReturnType() const override;

  CREATE_FACTORY(RegexExtractProvider)
 private:
  std::regex regex;
};
}  // namespace joda::query

#endif  // JODA_REGEXEXTRACTPROVIDER_H
