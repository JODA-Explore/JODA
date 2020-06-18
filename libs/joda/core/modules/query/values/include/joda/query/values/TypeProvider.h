//
// Created by Nico Schäfer on 12/7/17.
//

#ifndef JODA_TYPEPROVIDER_H
#define JODA_TYPEPROVIDER_H

#include "IValueProvider.h"
namespace joda::query {
/**
 * TYPE(), returns the type of the given value
 */
class TypeProvider : public joda::query::IValueProvider {
 public:
  explicit TypeProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters);
  std::unique_ptr<IValueProvider> duplicate() const override;

  bool isConst() const override;

  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;
  std::string getName() const override;
  IValueType getReturnType() const override;

  CREATE_FACTORY(TypeProvider)
};
}

#endif  // JODA_TYPEPROVIDER_H
