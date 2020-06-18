//
// Created by Nico Schäfer on 12/7/17.
//

#include "../include/joda/query/values/ListAttributesProvider.h"

std::unique_ptr<joda::query::IValueProvider> joda::query::ListAttributesProvider::duplicate() const {
  return std::make_unique<joda::query::ListAttributesProvider>(duplicateParameters());
}

bool joda::query::ListAttributesProvider::isConst() const {
  return false;
}

RJValue joda::query::ListAttributesProvider::getAtomValue(const RapidJsonDocument &json,
                                                          RJMemoryPoolAlloc &alloc) const {
  assert(isAtom() && "Did not check for atom before calling");
  RJValue val;

  if (params[0]->isObject() || params[0]->isAny()) {
    auto *target = params[0]->getValue(json, alloc);
    if (target != nullptr && target->IsObject()) {
      val.SetArray();
      for (auto &m : target->GetObject()) {
        RJValue attrName;
        attrName.SetString(m.name.GetString(), alloc);
        val.PushBack(std::move(attrName), alloc);
        if(recursive && m.value.IsObject()) recursiveAdd(m.name.GetString(),val,m.value,alloc);
      }
    }
  }
  return val;
}
const RJValue *joda::query::ListAttributesProvider::getValue(const RapidJsonDocument &json,
                                                             RJMemoryPoolAlloc &alloc) const {
  return nullptr;
}

bool joda::query::ListAttributesProvider::isAtom() const {
  return true;
}

joda::query::ListAttributesProvider::ListAttributesProvider(std::vector<std::unique_ptr<IValueProvider>> &&parameters): IValueProvider(
    std::move(parameters)) {
  if(params.size() == 0) throw joda::query::WrongParameterCountException(params.size(), 1, getName());
  else {
    checkParamType(0,IV_Object);
    if(params.size() > 2){
      throw joda::query::WrongParameterCountException(params.size(), 2, getName());
    }
    if(params.size() == 2){
      checkParamType(1,IV_Bool);
      if(!this->params[1]->isConst()) throw joda::query::WrongParameterException("Parameter 1 has to be const bool");
      RJMemoryPoolAlloc alloc;
      recursive = this->params[1]->getAtomValue(RapidJsonDocument(), alloc).GetBool();
    }
  }
}

std::string joda::query::ListAttributesProvider::getName() const {
  return "LISTATTRIBUTES";
}

joda::query::IValueType joda::query::ListAttributesProvider::getReturnType() const {
  return IV_Array;
}

void joda::query::ListAttributesProvider::recursiveAdd(const std::string &prefix,
                                                       RJValue &arr,
                                                       const RJValue &obj,
                                                       RJMemoryPoolAlloc &alloc) const{
  if(obj.IsObject()){
    for (auto &m : obj.GetObject()) {
      RJValue attrName;
      auto pref = prefix+"/"+m.name.GetString();
      attrName.SetString(pref.c_str(), alloc);
      arr.PushBack(std::move(attrName), alloc);
      if(m.value.IsObject()) recursiveAdd(pref,arr,m.value,alloc);
    }
  }
}
