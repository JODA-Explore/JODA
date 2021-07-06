//
// Created by Nico Schäfer on 11/27/17.
//

#ifndef JODA_INLISTPROVIDER_H
#define JODA_INLISTPROVIDER_H

#include <rapidjson/fwd.h>

#include <unordered_set>

#include "IValueProvider.h"

namespace joda::query {
/**
 * IN(X,Y) checks if X is in array Y
 */
class INListProvider : public joda::query::IValueProvider {
 public:
  std::unique_ptr<IValueProvider> duplicate() const override;

  bool isConst() const override;

  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;

  explicit INListProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters);
  std::string getName() const override;
  IValueType getReturnType() const override;

  CREATE_FACTORY(INListProvider)

 protected:
};
}  // namespace joda::query

#endif  // JODA_INLISTPROVIDER_H
