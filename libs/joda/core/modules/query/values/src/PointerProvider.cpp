//
// Created by Nico Schäfer on 11/13/17.
//

#include "../include/joda/query/values/PointerProvider.h"

#include <rapidjson/fwd.h>
#include <rapidjson/stringbuffer.h>
bool joda::query::PointerProvider::comparable() const { return true; }
bool joda::query::PointerProvider::equalizable() const { return true; }

joda::query::PointerProvider::PointerProvider(const std::string& str,
                                              bool prefixed)
    : pointer(str.c_str()), ptrStr(str), prefixed(prefixed) {}

std::unique_ptr<joda::query::IValueProvider>
joda::query::PointerProvider::duplicate() const {
  return std::make_unique<joda::query::PointerProvider>(ptrStr,
                                                        prefixed);
}
std::string joda::query::PointerProvider::toString() const {
  if (!prefixed) {
    return std::string("'") + ptrStr + "'";
  }
  return std::string("$'") + ptrStr + "'";
}

const std::string& joda::query::PointerProvider::getAttributeString() const {
  if(prefixed) return prefixedPtrStr;
  return ptrStr;
}

const RJPointer& joda::query::PointerProvider::getPointer() const {
  if(prefixed) return prefixedPtr;
  return pointer;
}

bool joda::query::PointerProvider::isConst() const { return false; }
RJValue joda::query::PointerProvider::getAtomValue(
    const RapidJsonDocument& /*json*/, RJMemoryPoolAlloc& /*alloc*/) const {
  assert(isAtom() && "Did not check for atom before calling");
  return RJValue();
}
const RJValue* joda::query::PointerProvider::getValue(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& /*alloc*/) const {
  assert(!isAtom() && "Did not check for atom before calling");
  return json.Get(getPointer());
}
bool joda::query::PointerProvider::isString() const { return false; }
bool joda::query::PointerProvider::isNumber() const { return false; }
bool joda::query::PointerProvider::isBool() const { return false; }
bool joda::query::PointerProvider::isObject() const { return false; }
bool joda::query::PointerProvider::isArray() const { return false; }
bool joda::query::PointerProvider::isAny() const { return true; }

std::string joda::query::PointerProvider::getName() const { return ""; }

joda::query::IValueType joda::query::PointerProvider::getReturnType() const {
  return IV_Any;
}

void joda::query::PointerProvider::getAttributes(
    std::vector<std::string>& vec) const {
  if (!prefixed) {
    vec.emplace_back(ptrStr);
  }
}

std::vector<std::string> joda::query::PointerProvider::getAttributes() const {
  std::vector<std::string> ret;
  getAttributes(ret);
  return ret;
}

bool joda::query::PointerProvider::objIsPointerEvaluatable(
    const RapidJsonDocument& json) const {
  return !json.isView() || json.getView()->pointerIsOverwritten(getAttributeString());
}

const VirtualObject* joda::query::PointerProvider::getVO(
    const RapidJsonDocument& json) const {
  auto& v = json.getView();
  if (v == nullptr) {
    return nullptr;
  }
  return v->getVO(getAttributeString());
}

std::variant<const RJValue, std::optional<const RJValue*>, const VirtualObject*>
joda::query::PointerProvider::getPointerIfExists(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& alloc) const {
  auto& v = json.getView();
  if (!json.isView() || v == nullptr) {
    auto* ret = getValue(json, alloc);
    return ret;
  }
  return v->getPointerIfExists(getAttributeString(), alloc);
}

void joda::query::PointerProvider::prependAttributes(
    const std::string& prefix) {
  if (!prefixed) {  // Only prefixed PointerProviders can be prefixed
    return;
  }
  prefixedPtrStr = prefix + ptrStr;
  prefixedPtr = RJPointer(prefixedPtrStr.c_str());
}
