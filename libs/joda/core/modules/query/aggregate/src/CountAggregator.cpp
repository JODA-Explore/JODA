//
// Created by Nico Schäfer on 10/30/17.
//

#include <rapidjson/document.h>
#include "../include/joda/query/aggregation/CountAggregator.h"

void joda::query::CountAggregator::merge(IAggregator *other) {
  auto *o = dynamic_cast<CountAggregator *>(other);
  assert(o != nullptr);
  assert(getName() == o->getName());
  assert(toPointer == o->toPointer);
  count += o->count;
}

RJValue joda::query::CountAggregator::terminate(RJMemoryPoolAlloc &alloc) {
  RJValue ret;
  ret.SetInt64(count);
  return ret;
}

std::unique_ptr<joda::query::IAggregator> joda::query::CountAggregator::duplicate() const {
  return std::make_unique<CountAggregator>(toPointer, duplicateParameters());
}

joda::query::CountAggregator::CountAggregator(const std::string &toPointer, std::vector<std::unique_ptr<IValueProvider>> &&params)
    : IAggregator(toPointer, std::move(params)) {
  if (this->params.size() > 1) throw WrongParameterCountException(this->params.size(), 1, getName());
}

void joda::query::CountAggregator::accumulate(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
  if (params.empty()) count++;
  else {
    const RJValue *val;
    RJValue val_;
    if (params[0]->isAtom()) {
      val_ = params[0]->getAtomValue(json, alloc);
      val = &val_;
    } else {
      val = params[0]->getValue(json, alloc);
    }
    if (val != nullptr) count++;
  }
}

const std::string joda::query::CountAggregator::getName() const {
  return getName_();
}


