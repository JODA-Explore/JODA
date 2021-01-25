//
// Created by Nico on 21/05/2019.
//

#include "joda/query/aggregation/GroupAggregator.h"

joda::query::GroupAggregator::GroupAggregator(const std::string &toPointer,
                                              std::unique_ptr<IValueProvider> &&groupBy,
                                              std::unique_ptr<IAggregator> &&agg, std::string groupAs)
    : IAggregator(toPointer, {}), groupBy(std::move(groupBy)), protoAgg(std::move(agg)), groupAs(groupAs) {

}

void joda::query::GroupAggregator::merge(IAggregator *other) {
  auto *o = dynamic_cast<joda::query::GroupAggregator *>(other);
  assert(o != nullptr);
  assert(getName() == o->getName());
  assert(toPointer == o->toPointer);
  for (auto &stringAgg : o->stringGroups) {
    auto iterator = stringGroups.find(stringAgg.first);
    if (iterator != stringGroups.end()) { // Aggregator already exists
      iterator->second->merge(stringAgg.second.get());
    } else { //Aggregator does not yet exist
      stringGroups[stringAgg.first] = std::move(stringAgg.second);
    }
  }

  for (auto &numAgg : o->numGroups) {
    auto iterator = numGroups.find(numAgg.first);
    if (iterator != numGroups.end()) { // Aggregator already exists
      iterator->second->merge(numAgg.second.get());
    } else { //Aggregator does not yet exist
      numGroups[numAgg.first] = std::move(numAgg.second);
    }
  }

  if (o->trueAgg != nullptr) {
    if (trueAgg != nullptr) {
      trueAgg->merge(o->trueAgg.get());
    } else {
      trueAgg = std::move(o->trueAgg);
    }
  }
  if (o->falseAgg != nullptr) {
    if (falseAgg != nullptr) {
      falseAgg->merge(o->falseAgg.get());
    } else {
      falseAgg = std::move(o->falseAgg);
    }
  }

}

std::unique_ptr<joda::query::IAggregator> joda::query::GroupAggregator::duplicate() const {
  return std::make_unique<joda::query::GroupAggregator>(toPointer, groupBy->duplicate(), protoAgg->duplicate(), groupAs);
}

const std::string joda::query::GroupAggregator::getName() const {
  return std::string(getName_()) + " " + protoAgg->getName();
}

std::string joda::query::GroupAggregator::toString() const {
  std::string asStr;
  if (!groupAs.empty()) asStr += "AS " + groupAs + " ";
  return "'" + toPointer + "':" + getName() + "(" + protoAgg->getParameterStringRepresentation() + ") " + asStr + "BY "
      + groupBy->toString();
}

const std::string joda::query::GroupAggregator::getGroupName() const {
  if (!groupAs.empty()) return groupAs;
  return protoAgg->getName() + "(" + protoAgg->getParameterStringRepresentation() + ")";
}

const std::string joda::query::GroupAggregator::getValueName() const {
  return "group";
}

RJValue joda::query::GroupAggregator::terminate(RJMemoryPoolAlloc &alloc) {

  RJValue val;
  val.SetArray();

  for (auto &&str : stringGroups) {
    RJValue obj(rapidjson::kObjectType);
    obj.AddMember({getValueName(), alloc}, {str.first.c_str(), alloc}, alloc);
    obj.AddMember({"groupedBy", alloc}, {groupBy->toString(), alloc}, alloc);
    obj.AddMember({getGroupName(), alloc}, str.second->terminate(alloc), alloc);
    val.PushBack(obj, alloc);
  }
  for (auto &&num : numGroups) {
    RJValue obj(rapidjson::kObjectType);
    obj.AddMember({getValueName(), alloc}, RJValue(num.first), alloc);
    obj.AddMember({"groupedBy", alloc}, {groupBy->toString(), alloc}, alloc);
    obj.AddMember({getGroupName(), alloc}, num.second->terminate(alloc), alloc);
    val.PushBack(obj, alloc);
  }
  if (trueAgg != nullptr) {
    RJValue obj(rapidjson::kObjectType);
    obj.AddMember({getValueName(), alloc}, RJValue(true), alloc);
    obj.AddMember({"groupedBy", alloc}, {groupBy->toString(), alloc}, alloc);
    obj.AddMember({getGroupName(), alloc}, trueAgg->terminate(alloc), alloc);
    val.PushBack(obj, alloc);
  }
  if (falseAgg != nullptr) {
    RJValue obj(rapidjson::kObjectType);
    obj.AddMember({getValueName(), alloc}, RJValue(false), alloc);
    obj.AddMember({"groupedBy", alloc}, {groupBy->toString(), alloc}, alloc);
    obj.AddMember({getGroupName(), alloc}, falseAgg->terminate(alloc), alloc);
    val.PushBack(obj, alloc);
  }
  return val;
}

void joda::query::GroupAggregator::accumulate(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
  const RJValue *val;
  RJValue val_;
  if (groupBy->isAtom()) {
    val_ = groupBy->getAtomValue(json, alloc);
    val = &val_;
  } else {
    val = groupBy->getValue(json, alloc);
  }
  if (val != nullptr) {
    if (val->IsString()) {
      auto str = val->GetString();
      auto it = stringGroups.find(str);
      if (it != stringGroups.end()) {
        it->second->accumulate(json, alloc);
      } else {
        auto agg = protoAgg->duplicate();
        agg->accumulate(json, alloc);
        stringGroups[str] = std::move(agg);
      }
    }

    if (val->IsNumber()) {
      auto num = val->GetDouble();
      auto it = numGroups.find(num);
      if (it != numGroups.end()) {
        it->second->accumulate(json, alloc);
      } else {
        auto agg = protoAgg->duplicate();
        agg->accumulate(json, alloc);
        numGroups[num] = std::move(agg);
      }
    }

    if (val->IsBool()) {
      if (val->IsTrue()) {
        if (trueAgg == nullptr) trueAgg = protoAgg->duplicate();
        trueAgg->accumulate(json, alloc);
      } else {
        DCHECK(val->IsFalse());
        if (falseAgg == nullptr) falseAgg = protoAgg->duplicate();
        falseAgg->accumulate(json, alloc);
      }
    }
  }
}

void joda::query::GroupAggregator::setGroupAs(const std::string &groupAs) {
  GroupAggregator::groupAs = groupAs;
}

std::vector<std::string> joda::query::GroupAggregator::getAttributes() const {
  auto attributes = protoAgg->getAttributes();
  groupBy->getAttributes(attributes);
  return attributes;
}


