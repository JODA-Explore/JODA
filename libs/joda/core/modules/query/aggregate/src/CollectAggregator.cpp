//
// Created by Nico on 18/03/2019.
//

#include "joda/query/aggregation/CollectAggregator.h"

#include <joda/query/aggregation/CollectAggregator.h>
#include <joda/query/aggregation/IAggregator.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

const std::string joda::query::CollectAggregator::getName() const {
  return getName_();
}

void joda::query::CollectAggregator::merge(IAggregator* other) {
  auto* o = dynamic_cast<CollectAggregator*>(other);
  assert(o != nullptr);
  assert(getName() == o->getName());
  assert(toPointer == o->toPointer);

  for (const auto& v : o->list->GetArray()) {
    RJValue copy(v, *alloc, true);
    list->PushBack(copy.Move(), *alloc);
  }
}

RJValue joda::query::CollectAggregator::terminate(
    RJMemoryPoolAlloc& term_alloc) {
  if (list != nullptr) {
    RJValue copy(*list, term_alloc, true);
    return copy;
  }
  return RJValue(rapidjson::kArrayType);
}

std::unique_ptr<joda::query::IAggregator>
joda::query::CollectAggregator::duplicate() const {
  return std::make_unique<CollectAggregator>(toPointer, duplicateParameters());
}

void joda::query::CollectAggregator::accumulate(const RapidJsonDocument& json,
                                                RJMemoryPoolAlloc& /*alloc*/) {
  RJValue val_;
  if (params[0]->isAtom()) {
    val_ = params[0]->getAtomValue(json, *this->alloc);
  } else {
    const auto* val = params[0]->getValue(json, *this->alloc);
    if (val != nullptr) {
      val_.CopyFrom(*val, *this->alloc, true);
    }
  }
  list->PushBack(std::move(val_), *this->alloc);
}

joda::query::CollectAggregator::CollectAggregator(
    const std::string& toPointer,
    std::vector<std::unique_ptr<IValueProvider>>&& params)
    : IAggregator(toPointer, std::move(params)) {
  list = std::make_unique<RJValue>(rapidjson::kArrayType);
  alloc = std::make_unique<RJMemoryPoolAlloc>();
  checkParamSize(1);
}
