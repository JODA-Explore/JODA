//
// Created by Nico on 08/05/2019.
//

#include "joda/query/values/NullProvider.h"

joda::query::NullProvider::NullProvider() = default;

joda::query::IValueType joda::query::NullProvider::getReturnType() const {
  return IV_Null;
}

std::unique_ptr<joda::query::IValueProvider>
joda::query::NullProvider::duplicate() const {
  return std::make_unique<joda::query::NullProvider>();
}

bool joda::query::NullProvider::isConst() const { return true; }

RJValue joda::query::NullProvider::getAtomValue(
    const RapidJsonDocument& /*json*/, RJMemoryPoolAlloc& /*alloc*/) const {
  return RJValue();
}

const RJValue* joda::query::NullProvider::getValue(
    const RapidJsonDocument& /*json*/, RJMemoryPoolAlloc& /*alloc*/) const {
  assert(isAtom() && "Did not check for atom before calling");
  return nullptr;
}

bool joda::query::NullProvider::isAtom() const { return true; }

std::string joda::query::NullProvider::getName() const { return "NULL"; }
