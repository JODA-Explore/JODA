

#ifndef JODA_REGEXEXTRACTFIRSTPROVIDER_H
#define JODA_REGEXEXTRACTFIRSTPROVIDER_H

#include <regex>

#include "IValueProvider.h"
namespace joda::query {
/**
 * REGEX_EXTRACT_FIRST(), extracts matching groups from a string, based on a regex
 */
class RegexExtractFirstProvider : public joda::query::IValueProvider {
 public:
  explicit RegexExtractFirstProvider(
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

  CREATE_FACTORY(RegexExtractFirstProvider)
 private:
  std::regex regex;
};
}  // namespace joda::query

#endif  // JODA_REGEXEXTRACTFIRSTPROVIDER_H
