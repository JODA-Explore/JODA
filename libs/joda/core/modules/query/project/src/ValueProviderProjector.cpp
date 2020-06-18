//
// Created by Nico Schäfer on 12/6/17.
//

#include "../include/joda/query/project/ValueProviderProjector.h"

joda::query::ValueProviderProjector::ValueProviderProjector(const std::string &to,
                                                            std::unique_ptr<IValueProvider> &&valprov)
    : IProjector(to), valprov(std::move(valprov)) {
  IValueProvider::replaceConstSubexpressions(this->valprov);
}

std::string joda::query::ValueProviderProjector::getType() {
  return type;
}
RJValue joda::query::ValueProviderProjector::getVal(const RapidJsonDocument &json,
                                                    RJMemoryPoolAlloc &alloc) {
  assert(valprov != nullptr);

  if (valprov->isAtom()) {
    return valprov->getAtomValue(json, alloc);
  }
  auto *tmp = valprov->getValue(json, alloc);
  RJValue val;
  if (tmp != nullptr)
    val.CopyFrom(*tmp, alloc);
  return val;
}

const std::string joda::query::ValueProviderProjector::type = "ValueProviderProjection";
std::string joda::query::ValueProviderProjector::toString() {
  return IProjector::toString() + valprov->toString();
}
