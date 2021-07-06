//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_NULLPROVIDER_H
#define JODA_NULLPROVIDER_H

#include "IValueProvider.h"

namespace joda::query {
/**
 * null, a constant null value
 */
class NullProvider : public joda::query::IValueProvider {
 public:
  NullProvider();
  std::unique_ptr<IValueProvider> duplicate() const override;

  bool isConst() const override;
  bool isAtom() const override;

  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;

  std::string getName() const override;
  IValueType getReturnType() const override;
};
}  // namespace joda::query

#endif  // JODA_NULLPROVIDER_H
