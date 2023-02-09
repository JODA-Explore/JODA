#include <utility>

//
// Created by Nico Schäfer on 11/13/17.
//

#ifndef JODA_ATOMPROVIDER_H
#define JODA_ATOMPROVIDER_H

#define JODA_TRUE_STRING "true"
#define JODA_FALSE_STRING "false"

#include <rapidjson/fwd.h>

#include "IValueProvider.h"
namespace joda::query {

/**
 * Provides constant typed values like ints, doubles, strings, and Booleans
 */
template <typename T>
class AtomProvider : public joda::query::IValueProvider {
 public:
  AtomProvider(T val)
      : val(std::move(val)){

        };

  std::unique_ptr<IValueProvider> duplicate() const override {
    return std::make_unique<AtomProvider<T>>(val);
  };
  std::string toString() const override;

  bool isConst() const override { return true; };
  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;

  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override {
    DCHECK(!isAtom()) << "Did not check for atom before calling";
    return nullptr;
  };

  std::string getName() const override;
  joda::query::IValueType getReturnType() const override;

 protected:
  T val;
};

template class AtomProvider<double>;

typedef AtomProvider<double> DoubleProvider;

template class AtomProvider<int64_t>;

typedef AtomProvider<int64_t> Int64Provider;

template class AtomProvider<u_int64_t>;

typedef AtomProvider<u_int64_t> UInt64Provider;

/*
 * Bool
 */

template class AtomProvider<bool>;

typedef AtomProvider<bool> BoolProvider;

template class AtomProvider<std::string>;

typedef AtomProvider<std::string> StringProvider;

/**
 * toString()
 */

template <>
std::string AtomProvider<std::string>::toString() const;

template <>
std::string AtomProvider<u_int64_t>::toString() const;

template <>
std::string AtomProvider<int64_t>::toString() const;

template <>
std::string AtomProvider<double>::toString() const;

template <>
std::string AtomProvider<bool>::toString() const;

template <typename T>
inline std::string AtomProvider<T>::getName() const {
  return toString();
};

}  // namespace joda::query
#endif  // JODA_ATOMPROVIDER_H
