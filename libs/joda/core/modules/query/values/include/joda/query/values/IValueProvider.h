//
// Created by Nico Schäfer on 11/13/17.
//

#ifndef JODA_IVALUEPROVIDER_H
#define JODA_IVALUEPROVIDER_H
#include <glog/logging.h>
#include <rapidjson/document.h>
#include <rapidjson/fwd.h>

#include <cmath>
#include <experimental/optional>
#include <memory>
#include <string>

#include "joda/document/RapidJsonDocument.h"

// Helper define, which creates a factory method used to create an instance of a
// function, given a list of parameters
#define CREATE_FACTORY(FCLASS)                                 \
  static std::unique_ptr<IValueProvider> _FACTORY(             \
      std::vector<std::unique_ptr<IValueProvider>> &&params) { \
    return std::make_unique<FCLASS>(std::move(params));        \
  }

namespace joda::query {
/**
 * Describes the return types of functions and parameters
 * Most are self-explanatory.
 *
 * IV_Any is used if the type can change on a per-document level.
 */
enum IValueType {
  IV_String,
  IV_Number,
  IV_Bool,
  IV_Object,
  IV_Array,
  IV_Any,
  IV_Null
};

/**
 * Generic Exception for errors regarding function parameters
 */
class WrongParameterException : public std::exception {
 public:
  WrongParameterException(const std::string &whatStr) : whatStr(whatStr) {}
  virtual const char *what() const throw() { return whatStr.c_str(); }

 protected:
  WrongParameterException() = default;
  std::string whatStr;
};

/**
 * Exception will be thrown if a type mismatches with the expected parameter
 * type
 */
class WrongParameterTypeException : public WrongParameterException {
 public:
  WrongParameterTypeException(unsigned int i, IValueType expected,
                              const std::string &name) {
    whatStr = name + ": Parameter " + std::to_string(i) +
              " is of wrong type. Expected: ";
    switch (expected) {
      case IV_String:
        whatStr += "String";
        break;
      case IV_Number:
        whatStr += "Number";
        break;
      case IV_Bool:
        whatStr += "Bool";
        break;
      case IV_Object:
        whatStr += "Object";
        break;
      case IV_Array:
        whatStr += "Array";
        break;
      case IV_Any:
        whatStr += "Any";
        break;
      case IV_Null:
        DCHECK(false) << "Expected null parameter? O.o";
        break;
    }
  }
};

/**
 * Exception will be thrown if too many or too few parameters are passed to a
 * function
 */
class WrongParameterCountException : public WrongParameterException {
 public:
  WrongParameterCountException(unsigned int count, unsigned int expected,
                               const std::string &name) {
    whatStr = name + ": Expected " + std::to_string(expected) +
              " parameters, but got " + std::to_string(count);
  }
};

/**
 * IValueProviders provide values to functions.
 * They may be constant values or functions themselves
 */
class IValueProvider {
 public:
  /**
   * Creates a new IValueProvider with given parameters
   * @throws WrongParameterException if the parameters do not fit the expected
   * (wrong count or type)
   * @param parameters A vector of IValueProviders
   */
  explicit IValueProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters);
  /**
   * Creates a new IValueProvider without parameters
   * @throws WrongParameterException If parameters where expected
   */
  IValueProvider();

  virtual ~IValueProvider() = default;

  /**
   * Gets an atomic value
   * @attention isAtom() has to return true. If not this will result in
   * undefined behavior
   * @param json The Rapidjsondocument to retrieve values from. (Or an empty
   * RapidJsonDocument for const values)
   * @param alloc A MemoryPoolAllocator to create the values with
   * @return a value containing the result
   */
  virtual RJValue getAtomValue(const RapidJsonDocument &json,
                               RJMemoryPoolAlloc &alloc) const = 0;

  /**
   * Gets an value
   * @attention isAtom() has to return false. If not this will result in
   * undefined behavior
   * @param json The Rapidjsondocument to retrieve values from. (Or an empty
   * RapidJsonDocument for const values)
   * @param alloc A MemoryPoolAllocator to create the values with
   * @return a value-pointer containing the result
   */
  virtual RJValue const *getValue(const RapidJsonDocument &json,
                                  RJMemoryPoolAlloc &alloc) const = 0;

  /**
   * @return a pointer to a duplicate of the IValueProvider
   */
  virtual std::unique_ptr<IValueProvider> duplicate() const = 0;
  /**
   * @return A textual representation of the IValueProvider
   */
  virtual std::string toString() const;
  /**
   *
   * @return The function name
   */
  virtual std::string getName() const = 0;
  /**
   *
   * @return True if the Provider is const, false else
   */
  virtual bool isConst() const = 0;
  /**
   *
   * @return The type returned by the function
   */
  virtual IValueType getReturnType() const = 0;
  virtual bool isString() const { return getReturnType() == IV_String; };
  virtual bool isNumber() const { return getReturnType() == IV_Number; };
  virtual bool isBool() const { return getReturnType() == IV_Bool; };
  virtual bool isObject() const { return getReturnType() == IV_Object; };
  virtual bool isArray() const { return getReturnType() == IV_Array; };
  virtual bool isNull() const { return getReturnType() == IV_Null; };
  virtual bool isAny() const { return getReturnType() == IV_Any; };
  virtual bool isAtom() const { return isString() || isNumber() || isBool(); };

  /**
   * Gets a list of all used document attributes
   *
   * @return List of used document attributes
   */
  virtual std::vector<std::string> getAttributes() const {
    std::vector<std::string> ret;
    for (auto &&param : params) {
      param->getAttributes(ret);
    }
    return ret;
  }

  /**
   * Fills a list with all used document attributes
   * @param vec list to fill
   */
  virtual void getAttributes(std::vector<std::string> &vec) const {
    for (auto &&param : params) {
      param->getAttributes(vec);
    }
  }

  /**
   * Checks if two IValueProviders values are equal
   * @param other other IValueProvider to compare with
   * @param json  The RapidJsonDocument to (potentially) retrieve values from
   * @return True if the values are equal, false otherwise
   */
  bool equal(IValueProvider *other, const RapidJsonDocument &json) const {
    RJMemoryPoolAlloc tmpAlloc;
    const RJValue *lhs;
    RJValue tmplhs;
    const RJValue *rhs;
    RJValue tmprhs;
    // Get Pointer to value
    if (isAtom()) {
      tmplhs = getAtomValue(json, tmpAlloc);
      lhs = &tmplhs;
    } else {
      lhs = getValue(json, tmpAlloc);
    }
    if (lhs == nullptr) return false;
    // Get Pointer to List
    if (other->isAtom()) {
      tmprhs = other->getAtomValue(json, tmpAlloc);
      rhs = &tmprhs;
    } else {
      rhs = other->getValue(json, tmpAlloc);
    }
    if (rhs == nullptr) return false;

    auto lhsType = lhs->GetType();
    auto rhsType = rhs->GetType();
    if (lhsType != rhsType) return false;  // If different types, false

    if (lhsType == rapidjson::kTrueType && rhsType == rapidjson::kTrueType)
      return true;
    if (lhsType == rapidjson::kFalseType && rhsType == rapidjson::kFalseType)
      return true;

    if (lhsType == rapidjson::kNumberType &&
        rhsType == rapidjson::kNumberType) {
      if (lhs->IsUint64() && rhs->IsUint64())
        return lhs->GetUint64() == rhs->GetUint64();
      if (lhs->IsInt64() && rhs->IsInt64())
        return lhs->GetInt64() == rhs->GetInt64();
      return fabs(lhs->GetDouble() - rhs->GetDouble()) <
             std::numeric_limits<double>::epsilon();
    }

    if (lhsType == rapidjson::kStringType &&
        rhsType == rapidjson::kStringType) {
      return strcmp(lhs->GetString(), rhs->GetString()) == 0;
    }

    if (lhsType == rapidjson::kNullType && rhsType == rapidjson::kNullType) {
      return true;
    }

    return *lhs == *rhs;  // Deep equality check
  }

  /**
   *
   * @return True if comparable by >=, >, <=, <. False otherwise
   */
  virtual bool comparable() const {
    return getReturnType() == IV_String || getReturnType() == IV_Number;
  };
  /**
   *
   * @return True if comparable by ==, !=. False otherwise
   */
  virtual bool equalizable() const {
    return getReturnType() == IV_String || getReturnType() == IV_Number ||
           getReturnType() == IV_Bool || getReturnType() == IV_Object ||
           getReturnType() == IV_Array;
  };

  static void replaceConstSubexpressions(std::unique_ptr<IValueProvider> &val);
  static bool constBoolCheck(std::unique_ptr<IValueProvider> &val);

 protected:
  bool getParamString(std::string &ret,
                      const std::unique_ptr<IValueProvider> &val,
                      const RapidJsonDocument &json) const {
    RJMemoryPoolAlloc alloc;
    if (!val->isAtom()) {
      auto *retPtr = val->getValue(json, alloc);
      if (retPtr != nullptr && retPtr->IsString()) {
        ret = retPtr->GetString();
        return true;
      }
    } else if (val->isString()) {
      auto retVal = val->getAtomValue(json, alloc);
      if (retVal.IsString()) {
        ret = retVal.GetString();
        return true;
      }
    }
    return false;
  }

  void checkParamSize(unsigned int expected);
  void checkParamType(unsigned int i, IValueType expected);

  std::vector<std::unique_ptr<IValueProvider>> duplicateParameters() const;

  std::string getParameterStringRepresentation() const;

  std::vector<std::unique_ptr<IValueProvider>> params;
};
}

#endif  // JODA_IVALUEPROVIDER_H
