//
// Created by Nico Schäfer on 11/13/17.
//

#include "../include/joda/query/values/PointerProvider.h"
#include <rapidjson/fwd.h>
#include <rapidjson/stringbuffer.h>
bool joda::query::PointerProvider::comparable() const {
  return true;
}
bool joda::query::PointerProvider::equalizable() const {
  return true;
}
joda::query::PointerProvider::PointerProvider(const std::string &str) : pointer(str.c_str()) {

}
std::unique_ptr<joda::query::IValueProvider> joda::query::PointerProvider::duplicate() const {
  return std::make_unique<joda::query::PointerProvider>(getAttributeString());
}
std::string joda::query::PointerProvider::toString() const {
  rapidjson::StringBuffer sb;
  pointer.Stringify(sb);
  return std::string("'") + sb.GetString() + "'";
}
std::string joda::query::PointerProvider::getAttributeString() const {
  rapidjson::StringBuffer sb;
  pointer.Stringify(sb);
  return sb.GetString();
}
bool joda::query::PointerProvider::isConst() const {
  return false;
}
RJValue joda::query::PointerProvider::getAtomValue(const RapidJsonDocument &json,
                                                   RJMemoryPoolAlloc &alloc) const {
  assert(isAtom() && "Did not check for atom before calling");
  return RJValue();
}
const RJValue *joda::query::PointerProvider::getValue(const RapidJsonDocument &json,
                                                      RJMemoryPoolAlloc &alloc) const {
  assert(!isAtom() && "Did not check for atom before calling");
  auto *val = pointer.Get(*json.getJson());
  return val;
}
bool joda::query::PointerProvider::isString() const {
  return false;
}
bool joda::query::PointerProvider::isNumber() const {
  return false;
}
bool joda::query::PointerProvider::isBool() const {
  return false;
}
bool joda::query::PointerProvider::isObject() const {
  return false;
}
bool joda::query::PointerProvider::isArray() const {
  return false;
}
bool joda::query::PointerProvider::isAny() const {
  return true;
}

std::string joda::query::PointerProvider::getName() const {
  return "";
}

joda::query::IValueType joda::query::PointerProvider::getReturnType() const {
  return IV_Any;
}

void joda::query::PointerProvider::getAttributes(std::vector<std::string> &vec) const {
  rapidjson::StringBuffer sb;
  pointer.Stringify(sb);
  vec.emplace_back(sb.GetString());
}

