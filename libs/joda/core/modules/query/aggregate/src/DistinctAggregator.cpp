//
// Created by Nico Schäfer on 11/14/17.
//

#include "../include/joda/query/aggregation/DistinctAggregator.h"
#include <rapidjson/document.h>
#include <rapidjson/fwd.h>

void joda::query::DistinctAggregator::merge(IAggregator* other) {
  auto* o = dynamic_cast<joda::query::DistinctAggregator*>(other);
  assert(o != nullptr);
  assert(getName() == o->getName());
  assert(toPointer == o->toPointer);
  strSet.insert(o->strSet.begin(), o->strSet.end());
  numSet.insert(o->numSet.begin(), o->numSet.end());
  hasTrue |= o->hasTrue;
  hasFalse |= o->hasFalse;
}
RJValue joda::query::DistinctAggregator::terminate(RJMemoryPoolAlloc& alloc) {
  RJValue val;
  val.SetArray();
  if (stringEnabled && !strSet.empty()) {
    for (auto&& str : strSet) {
      RJValue strVal;
      strVal.SetString(str.c_str(), alloc);
      val.PushBack(std::move(strVal), alloc);
    }
  }
  if (numEnabled && !numSet.empty()) {
    for (auto&& num : numSet) {
      RJValue numVal;
      numVal.SetDouble(num);
      val.PushBack(std::move(numVal), alloc);
    }
  }
  if (boolEnabled && (hasTrue || hasFalse)) {
    if (hasTrue) {
      RJValue trueVal;
      trueVal.SetBool(true);
      val.PushBack(trueVal, alloc);
    }
    if (hasFalse) {
      RJValue falseVal;
      falseVal.SetBool(false);
      val.PushBack(falseVal, alloc);
    }
  }
  return val;
}
std::unique_ptr<joda::query::IAggregator>
joda::query::DistinctAggregator::duplicate() const {
  return std::make_unique<joda::query::DistinctAggregator>(
      toPointer, duplicateParameters(), stringEnabled, boolEnabled, numEnabled);
}

joda::query::DistinctAggregator::DistinctAggregator(
    const std::string& toPointer,
    std::vector<std::unique_ptr<IValueProvider>>&& params, bool stringEnabled,
    bool boolEnabled, bool numEnabled)
    : IAggregator(toPointer, std::move(params)),
      stringEnabled(stringEnabled),
      boolEnabled(boolEnabled),
      numEnabled(numEnabled) {
  checkParamSize(1);
}

void joda::query::DistinctAggregator::accumulate(const RapidJsonDocument& json,
                                                 RJMemoryPoolAlloc& alloc) {
  const RJValue* val;
  RJValue val_;
  if (params[0]->isAtom()) {
    val_ = params[0]->getAtomValue(json, alloc);
    val = &val_;
  } else {
    val = params[0]->getValue(json, alloc);
  }
  if (val != nullptr) {
    if (stringEnabled && val->IsString()) {
      strSet.insert(val->GetString());
    } else if (boolEnabled && val->IsBool()) {
      if (val->GetBool()) {
        hasTrue = true;
      } else {
        hasFalse = true;
      }
    } else if (numEnabled && val->IsNumber()) {
      numSet.insert(val->GetDouble());
    }
  }
}

const std::string joda::query::DistinctAggregator::getName() const {
  return getName_();
}
