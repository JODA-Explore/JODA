//
// Created by Nico Schäfer on 11/27/17.
//

#ifndef JODA_ARRAYSIZEPROVIDER_H
#define JODA_ARRAYSIZEPROVIDER_H

#include <rapidjson/fwd.h>

#include "IValueProvider.h"

namespace joda::query {
/**
 * SIZE() function.
 * Returns the count of entries in a given array
 */
class ArraySizeProvider : public joda::query::IValueProvider {
 public:
  explicit ArraySizeProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters);
  ArraySizeProvider();
  std::string getName() const override;
  IValueType getReturnType() const override;
  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;

  std::unique_ptr<IValueProvider> duplicate() const override;
  bool isConst() const override;

  CREATE_FACTORY(ArraySizeProvider)
 protected:
};
}

#endif  // JODA_ARRAYSIZEPROVIDER_H
