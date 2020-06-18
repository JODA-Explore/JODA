//
// Created by Nico Schäfer on 12/7/17.
//

#ifndef JODA_IDPROVIDER_H
#define JODA_IDPROVIDER_H

#include "IValueProvider.h"

namespace joda::query {
/**
 * ID() retrieves the document internal ID
 */
class IDProvider : public joda::query::IValueProvider {
 public:
  explicit IDProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters);
  std::unique_ptr<IValueProvider> duplicate() const override;

  bool isConst() const override;

  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;

  std::string getName() const override;
  IValueType getReturnType() const override;

  CREATE_FACTORY(IDProvider)
};
}

#endif  // JODA_IDPROVIDER_H
