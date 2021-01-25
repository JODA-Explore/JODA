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

joda::query::PointerProvider::PointerProvider(const std::string &str) : pointer(str.c_str()), ptrStr(str) {

}

std::unique_ptr<joda::query::IValueProvider> joda::query::PointerProvider::duplicate() const {
  return std::make_unique<joda::query::PointerProvider>(getAttributeString());
}
std::string joda::query::PointerProvider::toString() const {
  return std::string("'") + ptrStr + "'";
}

std::string joda::query::PointerProvider::getAttributeString() const {
  return ptrStr;
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
  return json.Get(pointer);
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
  vec.emplace_back(ptrStr);
}

std::vector<std::string> joda::query::PointerProvider::getAttributes() const {
  std::vector<std::string> ret;
  getAttributes(ret);
  return ret;
}

bool joda::query::PointerProvider::objIsPointerEvaluatable(const RapidJsonDocument &json) const {
  return !json.isView() || json.getView()->pointerIsOverwritten(ptrStr);
}

const VirtualObject *joda::query::PointerProvider::getVO(const RapidJsonDocument &json) const {
  auto &v = json.getView();
  if (v == nullptr) return nullptr;
  return v->getVO(ptrStr);
}

std::variant<const RJValue,
             std::optional<const RJValue *>,
             const VirtualObject *> joda::query::PointerProvider::getPointerIfExists(const RapidJsonDocument &json,
                                                                        RJMemoryPoolAlloc &alloc) const {
  auto &v = json.getView();
  if (!json.isView() || v == nullptr) {
    auto *ret = getValue(json, alloc);
    return ret;
  }
  return v->getPointerIfExists(ptrStr, alloc);
}

