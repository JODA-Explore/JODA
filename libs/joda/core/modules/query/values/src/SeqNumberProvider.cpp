//
// Created by Nico Schäfer on 12/7/17.
//

#include <utility>

#include "../include/joda/query/values/SeqNumberProvider.h"

std::unique_ptr<joda::query::IValueProvider>
joda::query::SeqNumberProvider::duplicate() const {
  return std::make_unique<joda::query::SeqNumberProvider>(currNum);
}

bool joda::query::SeqNumberProvider::isConst() const { return false; }

joda::query::SeqNumberProvider::SeqNumberProvider() {
  currNum = std::make_shared<std::atomic_ulong>();
}
joda::query::SeqNumberProvider::SeqNumberProvider(
    std::shared_ptr<std::atomic_ulong> currNum)
    : currNum(std::move(currNum)) {}
RJValue joda::query::SeqNumberProvider::getAtomValue(
    const RapidJsonDocument& /*json*/, RJMemoryPoolAlloc& /*alloc*/) const {
  assert(isAtom() && "Did not check for atom before calling");
  RJValue val;
  val.SetInt64(currNum->fetch_add(1));
  return val;
}
const RJValue* joda::query::SeqNumberProvider::getValue(
    const RapidJsonDocument& /*json*/, RJMemoryPoolAlloc& /*alloc*/) const {
  assert(!isAtom() && "Did not check for atom before calling");
  return nullptr;
}

joda::query::SeqNumberProvider::SeqNumberProvider(
    std::vector<std::unique_ptr<joda::query::IValueProvider>>&& parameters)
    : joda::query::IValueProvider(std::move(parameters)) {
  checkParamSize(0);
  currNum = std::make_shared<std::atomic_ulong>();
}

std::string joda::query::SeqNumberProvider::getName() const { return "SEQNUM"; }

joda::query::IValueType joda::query::SeqNumberProvider::getReturnType() const {
  return IV_Number;
}
