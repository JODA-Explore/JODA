//
// Created by Nico Schäfer on 12/6/17.
//

#include "../../include/joda/query/project/FlattenProjector.h"

void joda::query::FlattenProjector::project(const RapidJsonDocument &json,
                                            std::vector<std::shared_ptr<RJDocument>> &newDocs) {
  RJValue tmpVal;
  const RJValue *val;
  if (from->isAtom()) {
    tmpVal = from->getAtomValue(json, newDocs.front()->GetAllocator());
    val = &tmpVal;
  } else {
    val = from->getValue(json, newDocs.front()->GetAllocator());
  }

  if (val != nullptr && val->IsArray() && !val->Empty() && !newDocs.empty()) {
    auto count = val->Size();

    //Multiplicate array
    auto origDocs = multiplicate(newDocs, count);

    //Fill parts of array with new value
    auto *alloc = &newDocs.front()->GetAllocator();
    for (int i = 0; i < count; ++i) {
      auto arrptrstr = std::string("/" + std::to_string(i));
      RJPointer arrptr(arrptrstr.c_str());
      RJValue newval;
      newval.CopyFrom(*arrptr.Get(*val), *alloc);
      fillArrayRangeWithVal(newDocs, origDocs, i, newval);
    }
  }
}

std::string joda::query::FlattenProjector::getType() {
  return type;
}

const std::string joda::query::FlattenProjector::type = "FlattenProjector";
joda::query::FlattenProjector::FlattenProjector(const std::string &to, std::unique_ptr<IValueProvider> &&from)
    : joda::query::ISetProjector(to), from(std::move(from)) {

}
std::string joda::query::FlattenProjector::toString() {
  return joda::query::ISetProjector::toString() + "FLATTEN(" + from->toString() + ")";
}
