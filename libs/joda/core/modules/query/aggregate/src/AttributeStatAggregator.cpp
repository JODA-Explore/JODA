//
// Created by Nico Schäfer on 10/31/17.
//

#include "../include/joda/query/aggregation/AttributeStatAggregator.h"
#include <joda/query/values/ValueAccepter.h>
#include <rapidjson/document.h>

void joda::query::AttributeStatAggregator::merge(IAggregator* other) {
  auto* o = dynamic_cast<AttributeStatAggregator*>(other);
  assert(o != nullptr);
  assert(getName() == o->getName());
  assert(toPointer == o->toPointer);
  auto oNode = o->handler.finish();
  handler.merge(oNode);
}
RJValue joda::query::AttributeStatAggregator::terminate(
    RJMemoryPoolAlloc& alloc) {
  auto node = handler.finish();
  return node.toValue(alloc);
}
std::unique_ptr<joda::query::IAggregator>
joda::query::AttributeStatAggregator::duplicate() const {
  return std::make_unique<AttributeStatAggregator>(toPointer,
                                                   duplicateParameters());
}

joda::query::AttributeStatAggregator::AttributeStatAggregator(
    const std::string& toPointer,
    std::vector<std::unique_ptr<IValueProvider>>&& params)
    : IAggregator(toPointer, std::move(params)) {
  checkParamSize(1);
  checkParamType(0, IV_Any);
}

void joda::query::AttributeStatAggregator::accumulate(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& alloc) {
  ValueAccepter::Accept(params[0], json, alloc, handler);
  handler.finishDocument();
}

const std::string joda::query::AttributeStatAggregator::getName() const {
  return getName_();
}

std::vector<std::string> joda::query::AttributeStatAggregator::getAttributes()
    const {
  return {};
}
