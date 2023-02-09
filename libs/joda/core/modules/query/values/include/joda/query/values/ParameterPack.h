#ifndef JODA_PARAMETERPACK_H
#define JODA_PARAMETERPACK_H
#include <cstdint>

#include "ValueAccepter.h"
namespace joda::query::values {

class IncompatibleTypeException : public WrongParameterException {
 public:
  IncompatibleTypeException() {}
};

/**
 * No parameter, the default placeholder class
 * @tparam k the index of the parameter
 **/
template <std::size_t k>
struct NoParameter {
  static constexpr void check(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const std::string &name) {
    if (k < parameters.size())
      throw WrongParameterCountException(parameters.size(), k, name);
    return;
  }
};

/**
 * Wraps an parameter and accepts if it does not exist
 * @tparam k the index of the parameter
 * @tparam T the type of the wrapped parameter
 * @throws WrongParameterException if there is a problem with the parameter
 **/
template <std::size_t k, class T>
struct OptionalParameter {
  typedef std::optional<typename T::ReturnT> ReturnT;

  static void check(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const std::string &name) {
    if (k >= parameters.size()) return;
    return T::check(parameters, name);
  }

  static ReturnT extractValue(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    if (k >= parameters.size()) return {};
    return std::make_optional(T::extractValue(parameters));
  }
};

/**
 * Wraps an parameter and accepts if it does not exist
 * @tparam k the index of the parameter
 * @tparam T the type of the wrapped parameter
 * @throws WrongParameterException if there is a problem with the parameter
 **/
template <std::size_t k, class T, class Default>
struct OptionalWithDefaultParameter {
  typedef typename T::ReturnT ReturnT;

  static void check(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const std::string &name) {
    if (k >= parameters.size()) return;
    return T::check(parameters, name);
  }

  static ReturnT extractValue(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    if (k >= parameters.size()) return Default::value;
    return T::extractValue(parameters, json, alloc);
  }
};

struct TrueDefault {
  static constexpr bool value = true;
};

struct FalseDefault {
  static constexpr bool value = false;
};

/**
 * Wraps two parameters and combines them
 * @tparam k the index of the parameter
 * @tparam T1 the type of the first wrapped parameter
 * @tparam T2 the type of the second wrapped parameter
 * @throws WrongParameterException if there is a problem with the parameter
 **/
template <std::size_t k, class T1, class T2, class Default>
struct OrParameter {
  typedef typename std::variant<typename T1::ReturnT, typename T2::ReturnT>
      ReturnT;

  static void check(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const std::string &name) {
    try {  // Catch first wrong parameter exception
      T1::check(parameters, name);
    } catch (WrongParameterTypeException w) {
      T2::check(parameters, name);  // Do not catch second
    }
  }

  static ReturnT extractValue(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    try {  // Catch first wrong parameter exception
      return T1::extractValue(parameters, json, alloc);
    } catch (IncompatibleTypeException e) {
      T2::extractValue(parameters, json, alloc);  // Do not catch second
    }
  }
};

/**
 * A parameter of type String
 * @tparam k the index of the parameter
 * @throws WrongParameterException if there is a problem with the parameter
 **/
template <std::size_t k>
struct StringParameter {
  typedef std::string ReturnT;

  static void check(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const std::string &name) {
    if (k >= parameters.size()) throw MissingParameterException(k, name);
    if (!(parameters[k]->isString() || parameters[k]->isAny()))
      throw WrongParameterTypeException(k, IValueType::IV_String, name);
    return;
  }

  static ReturnT extractValue(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    auto &val = parameters[k];
    if (!val->isAtom()) {
      auto *retPtr = val->getValue(json, alloc);
      if (retPtr != nullptr && retPtr->IsString()) {
        return retPtr->GetString();
      }
    } else if (val->isString()) {
      auto retVal = val->getAtomValue(json, alloc);
      if (retVal.IsString()) {
        return retVal.GetString();
      }
    }
    throw IncompatibleTypeException();
  }
};

/**
 * A parameter of type Boolean
 * @tparam k the index of the parameter
 * @throws WrongParameterException if there is a problem with the parameter
 **/
template <std::size_t k>
struct BoolParameter {
  typedef bool ReturnT;

  static void check(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const std::string &name) {
    if (k >= parameters.size()) throw MissingParameterException(k, name);
    if (!(parameters[k]->isBool() || parameters[k]->isAny()))
      throw WrongParameterTypeException(k, IValueType::IV_Bool, name);
    return;
  }

  static ReturnT extractValue(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    auto &val = parameters[k];
    if (!val->isAtom()) {
      auto *retPtr = val->getValue(json, alloc);
      if (retPtr != nullptr && retPtr->IsBool()) {
        return retPtr->GetBool();
      }
    } else if (val->isBool()) {
      auto retVal = val->getAtomValue(json, alloc);
      if (retVal.IsBool()) {
        return retVal.GetBool();
      }
    }
    throw IncompatibleTypeException();
  }
};

/**
 * A parameter of type Object
 * @tparam k the index of the parameter
 * @throws WrongParameterException if there is a problem with the parameter
 **/
template <std::size_t k>
struct ObjectParameter {
typedef ValueAccepter ReturnT;
  static void check(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const std::string &name) {
    if (k >= parameters.size()) throw MissingParameterException(k, name);
    if (!(parameters[k]->isObject() || parameters[k]->isAny()))
      throw WrongParameterTypeException(k, IValueType::IV_Object, name);
    return;
  }

  static ReturnT extractValue(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    auto &param = parameters[k];
    return ValueAccepter(param);
  }

  template <class Handler>
  static bool accept(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      Handler &handler, const RapidJsonDocument &json,
      RJMemoryPoolAlloc &alloc) {
    auto &param = parameters[k];
    ValueAccepter accepter(param);
    if (param->isAtom()) {
      auto v = param->getAtomValue(json, alloc);
      return v.Accept(handler);
    }
    return accepter.Accept(param, json, alloc, handler);
  }
};

/**
 * A parameter of type Array
 * @tparam k the index of the parameter
 * @throws WrongParameterException if there is a problem with the parameter
 **/
template <std::size_t k>
struct ArrayParameter {
typedef ValueAccepter ReturnT;
  static void check(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const std::string &name) {
    if (k >= parameters.size()) throw MissingParameterException(k, name);
    if (!(parameters[k]->isArray() || parameters[k]->isAny()))
      throw WrongParameterTypeException(k, IValueType::IV_Array, name);
    return;
  }

  static ReturnT extractValue(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    auto &param = parameters[k];
    return ValueAccepter(param);
  }

  template <class Handler>
  static bool accept(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      Handler &handler, const RapidJsonDocument &json,
      RJMemoryPoolAlloc &alloc) {
    auto &param = parameters[k];
    ValueAccepter accepter(param);
    if (param->isAtom()) {
      auto v = param->getAtomValue(json, alloc);
      return v.Accept(handler);
    }
    return accepter.Accept(param, json, alloc, handler);
  }
};

/**
 * A parameter of type Object or Array
 * @tparam k the index of the parameter
 * @throws WrongParameterException if there is a problem with the parameter
 **/
template <std::size_t k>
struct IteratableParameter {
typedef ValueAccepter ReturnT;
  static void check(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const std::string &name) {
    if (k >= parameters.size()) throw MissingParameterException(k, name);
    if (!(parameters[k]->isObject() || parameters[k]->isArray() ||
          parameters[k]->isAny())) {
      std::vector<IValueType> types = {IValueType::IV_Object,
                                       IValueType::IV_Array};
      throw WrongParameterTypeException(k, types, name);
    }
    return;
  }

  static ReturnT extractValue(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    auto &param = parameters[k];
    return ValueAccepter(param);
  }

  template <class Handler>
  static bool accept(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      Handler &handler, const RapidJsonDocument &json,
      RJMemoryPoolAlloc &alloc) {
    auto &param = parameters[k];
    ValueAccepter accepter(param);
    if (param->isAtom()) {
      auto v = param->getAtomValue(json, alloc);
      return v.Accept(handler);
    }
    return accepter.Accept(param, json, alloc, handler);
  }
};

/**
 * A parameter of type Any
 * @tparam k the index of the parameter
 * @throws WrongParameterException if there is a problem with the parameter
 **/
template <std::size_t k>
struct AnyParameter {
  typedef ValueAccepter ReturnT;

  static void check(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const std::string &name) {
    if (k >= parameters.size()) throw MissingParameterException(k, name);
    return;
  }

  static ReturnT extractValue(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    auto &param = parameters[k];
    return ValueAccepter(param);
  }

  template <class Handler>
  static bool accept(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      Handler &handler, const RapidJsonDocument &json,
      RJMemoryPoolAlloc &alloc) {
    auto &param = parameters[k];
    ValueAccepter accepter(param);
    if (param->isAtom()) {
      auto v = param->getAtomValue(json, alloc);
      return v.Accept(handler);
    }
    return accepter.Accept(param, json, alloc, handler);
  }
};

}  // namespace joda::query::values

#endif  // JODA_PARAMETERPACK_H