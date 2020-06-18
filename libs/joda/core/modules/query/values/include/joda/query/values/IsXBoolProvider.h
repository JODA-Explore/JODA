//
// Created by Nico Schäfer on 11/24/17.
//

#ifndef JODA_ISXBOOLPROVIDER_H
#define JODA_ISXBOOLPROVIDER_H
#include <rapidjson/fwd.h>

#include "IValueProvider.h"

namespace joda::query {
/**
 * ISARRAY()/ISNUMBER()/... checks if the value is of given type
 * There is one implementation for each IValueType
 *
 * Special case is IV_ANY, as this will be the EXISTS() function, which only checks if a given value exists at all.
 */
template <joda::query::IValueType T>
class IsXBoolProvider : public joda::query::IValueProvider {
 public:
  explicit IsXBoolProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters)
      : IValueProvider(std::move(parameters)) {
    checkParamSize(1);

    if (!params[0]->isAny() && params[0]->isConst()) {
      cons = true;
      consRes = T == IV_Any || T == params[0]->getReturnType();
    }
  }

  std::unique_ptr<IValueProvider> duplicate() const override {
    return std::make_unique<IsXBoolProvider>(duplicateParameters());
  }

  bool isConst() const override { return cons; }

  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;

  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override {
    assert(!isAtom() && "Did not check for atom before calling");
    return nullptr;
  }

  std::string getName() const override;

  joda::query::IValueType getReturnType() const override { return IV_Bool; }

  CREATE_FACTORY(IsXBoolProvider<T>)

 protected:
  bool cons = false;
  bool consRes = false;
};
}

#endif  // JODA_ISXBOOLPROVIDER_H
