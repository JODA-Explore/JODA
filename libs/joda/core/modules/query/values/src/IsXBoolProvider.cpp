#include "../include/joda/query/values/IsXBoolProvider.h"

template <>
std::string joda::query::IsXBoolProvider<joda::query::IV_Null>::getName()
    const {
  return "ISNULL";
}

template <>
std::string joda::query::IsXBoolProvider<joda::query::IV_Bool>::getName()
    const {
  return "ISBOOL";
}

template <>
std::string joda::query::IsXBoolProvider<joda::query::IV_Any>::getName() const {
  return "EXISTS";
}

template <>
std::string joda::query::IsXBoolProvider<joda::query::IV_Object>::getName()
    const {
  return "ISOBJECT";
}

template <>
std::string joda::query::IsXBoolProvider<joda::query::IV_Array>::getName()
    const {
  return "ISARRAY";
}

template <>
std::string joda::query::IsXBoolProvider<joda::query::IV_Number>::getName()
    const {
  return "ISNUMBER";
}

template <>
std::string joda::query::IsXBoolProvider<joda::query::IV_String>::getName()
    const {
  return "ISSTRING";
}

template <>
RJValue joda::query::IsXBoolProvider<joda::query::IV_Null>::getAtomValue(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& /*alloc*/) const {
  assert(isAtom() && "Did not check for atom before calling");
  if (cons) {
    RJValue val;
    val.SetBool(consRes);
    return val;
  }

  RJValue val;
  RJValue const* ptrVal;
  RJValue ptrValunref;
  RJMemoryPoolAlloc tmpalloc;

  if (params[0]->isAtom()) {
    ptrValunref = params[0]->getAtomValue(json, tmpalloc);
    ptrVal = &ptrValunref;
  } else {
    ptrVal = params[0]->getValue(json, tmpalloc);
  }
  if (ptrVal == nullptr) {
    val.SetBool(false);
    return val;
  }
  val.SetBool(ptrVal->IsNull());
  return val;
}

template <>
RJValue joda::query::IsXBoolProvider<joda::query::IV_Bool>::getAtomValue(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& /*alloc*/) const {
  assert(isAtom() && "Did not check for atom before calling");
  if (cons) {
    RJValue val;
    val.SetBool(consRes);
    return val;
  }

  RJValue val;
  RJValue const* ptrVal;
  RJValue ptrValunref;
  RJMemoryPoolAlloc tmpalloc;

  if (params[0]->isAtom()) {
    ptrValunref = params[0]->getAtomValue(json, tmpalloc);
    ptrVal = &ptrValunref;
  } else {
    ptrVal = params[0]->getValue(json, tmpalloc);
  }
  if (ptrVal == nullptr) {
    val.SetBool(false);
    return val;
  }
  val.SetBool(ptrVal->IsBool());
  return val;
}

template <>
RJValue joda::query::IsXBoolProvider<joda::query::IV_Any>::getAtomValue(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& /*alloc*/) const {
  assert(isAtom() && "Did not check for atom before calling");
  if (cons) {
    RJValue val;
    val.SetBool(consRes);
    return val;
  }

  RJValue val;
  RJValue const* ptrVal;
  RJValue ptrValunref;
  RJMemoryPoolAlloc tmpalloc;

  if (params[0]->isAtom()) {
    ptrValunref = params[0]->getAtomValue(json, tmpalloc);
    ptrVal = &ptrValunref;
  } else {
    ptrVal = params[0]->getValue(json, tmpalloc);
  }
  if (ptrVal == nullptr) {
    val.SetBool(false);
    return val;
  }
  val.SetBool(true);
  return val;
}

template <>
RJValue joda::query::IsXBoolProvider<joda::query::IV_Object>::getAtomValue(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& /*alloc*/) const {
  assert(isAtom() && "Did not check for atom before calling");
  if (cons) {
    RJValue val;
    val.SetBool(consRes);
    return val;
  }

  RJValue val;
  RJValue const* ptrVal;
  RJValue ptrValunref;
  RJMemoryPoolAlloc tmpalloc;

  if (params[0]->isAtom()) {
    ptrValunref = params[0]->getAtomValue(json, tmpalloc);
    ptrVal = &ptrValunref;
  } else {
    ptrVal = params[0]->getValue(json, tmpalloc);
  }
  if (ptrVal == nullptr) {
    val.SetBool(false);
    return val;
  }
  val.SetBool(ptrVal->IsObject());
  return val;
}

template <>
RJValue joda::query::IsXBoolProvider<joda::query::IV_Array>::getAtomValue(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& /*alloc*/) const {
  assert(isAtom() && "Did not check for atom before calling");
  if (cons) {
    RJValue val;
    val.SetBool(consRes);
    return val;
  }

  RJValue val;
  RJValue const* ptrVal;
  RJValue ptrValunref;
  RJMemoryPoolAlloc tmpalloc;

  if (params[0]->isAtom()) {
    ptrValunref = params[0]->getAtomValue(json, tmpalloc);
    ptrVal = &ptrValunref;
  } else {
    ptrVal = params[0]->getValue(json, tmpalloc);
  }
  if (ptrVal == nullptr) {
    val.SetBool(false);
    return val;
  }
  val.SetBool(ptrVal->IsArray());
  return val;
}

template <>
RJValue joda::query::IsXBoolProvider<joda::query::IV_Number>::getAtomValue(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& /*alloc*/) const {
  assert(isAtom() && "Did not check for atom before calling");
  if (cons) {
    RJValue val;
    val.SetBool(consRes);
    return val;
  }

  RJValue val;
  RJValue const* ptrVal;
  RJValue ptrValunref;
  RJMemoryPoolAlloc tmpalloc;

  if (params[0]->isAtom()) {
    ptrValunref = params[0]->getAtomValue(json, tmpalloc);
    ptrVal = &ptrValunref;
  } else {
    ptrVal = params[0]->getValue(json, tmpalloc);
  }
  if (ptrVal == nullptr) {
    val.SetBool(false);
    return val;
  }
  val.SetBool(ptrVal->IsNumber());
  return val;
}

template <>
RJValue joda::query::IsXBoolProvider<joda::query::IV_String>::getAtomValue(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& /*alloc*/) const {
  assert(isAtom() && "Did not check for atom before calling");
  if (cons) {
    RJValue val;
    val.SetBool(consRes);
    return val;
  }

  RJValue val;
  RJValue const* ptrVal;
  RJValue ptrValunref;
  RJMemoryPoolAlloc tmpalloc;

  if (params[0]->isAtom()) {
    ptrValunref = params[0]->getAtomValue(json, tmpalloc);
    ptrVal = &ptrValunref;
  } else {
    ptrVal = params[0]->getValue(json, tmpalloc);
  }
  if (ptrVal == nullptr) {
    val.SetBool(false);
    return val;
  }
  val.SetBool(ptrVal->IsString());
  return val;
}