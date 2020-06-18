//
// Created by Nico Schäfer on 11/13/17.
//

#include "joda/query/values/AtomProvider.h"

/*
 * Double
 */
template<>
std::string joda::query::AtomProvider<double>::toString() const {
  return std::to_string(val);
}

template<>
RJValue joda::query::AtomProvider<double>::getAtomValue(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) const {
  return RJValue(val);
}

template<>
joda::query::IValueType joda::query::AtomProvider<double>::getReturnType() const {
  return IV_Number;
}

/*
 * int64
 */
template<>
std::string joda::query::AtomProvider<int64_t>::toString() const {
  return std::to_string(val);
}

template<>
RJValue joda::query::AtomProvider<int64_t>::getAtomValue(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) const {
  return RJValue(val);
}

template<>
joda::query::IValueType joda::query::AtomProvider<int64_t>::getReturnType() const {
  return IV_Number;
}

/*
 * u_int64
 */
template<>
std::string joda::query::AtomProvider<u_int64_t>::toString() const {
  return std::to_string(val);
}

template<>
RJValue joda::query::AtomProvider<u_int64_t>::getAtomValue(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) const {
  return RJValue(val);
}

template<>
joda::query::IValueType joda::query::AtomProvider<u_int64_t>::getReturnType() const {
  return IV_Number;
}

template<>
std::string joda::query::AtomProvider<bool>::toString() const {
  if (val) return JODA_TRUE_STRING;
  return JODA_FALSE_STRING;
}

template<>
RJValue joda::query::AtomProvider<bool>::getAtomValue(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) const {
  return RJValue(val);
}

template<>
joda::query::IValueType joda::query::AtomProvider<bool>::getReturnType() const {
  return IV_Bool;
}

/*
 * String
 */
template<>
std::string joda::query::AtomProvider<std::string>::toString() const {
  return std::string("\"") + val + "\"";
}

template<>
RJValue joda::query::AtomProvider<std::string>::getAtomValue(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) const {
  return RJValue(val.c_str(), alloc);
}

template<>
joda::query::IValueType joda::query::AtomProvider<std::string>::getReturnType() const {
  return IV_String;
}