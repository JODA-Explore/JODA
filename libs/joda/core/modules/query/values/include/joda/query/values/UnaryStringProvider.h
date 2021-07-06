//
// Created by Nico on 06/09/2018.
//

#ifndef JODA_UNARYSTRINGPROVIDER_H
#define JODA_UNARYSTRINGPROVIDER_H

#include <joda/misc/RJFwd.h>
#include "IValueProvider.h"

namespace joda::query {
/**
 * Template class used for all unary string functions.
 * The template argument has to be a struct with the following
 * attributes/functions.
 * @code{.cpp}
 * struct <StructName> {
 *  static constexpr auto name = "<Name>";
 *  static constexpr IValueType retType = <IValueType returntype>;
 *  inline static RJValue calculate(std::string &&val, RJMemoryPoolAlloc &alloc)
 * {  return <result>; };
 * };
 * @endcode
 */
template <class Calc>
class UnaryStringProvider : public joda::query::IValueProvider {
 public:
  explicit UnaryStringProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters)
      : IValueProvider(std::move(parameters)) {
    checkParamSize(1);
    checkAllParamTypes();
    DCHECK(isAtom()) << "Only atom ReturnTypes allowed";
  };

  joda::query::IValueType getReturnType() const override {
    return Calc::retType;
  }

  std::string getName() const override { return Calc::name; };

  std::string toString() const override { return IValueProvider::toString(); }

  std::unique_ptr<IValueProvider> duplicate() const override {
    return std::make_unique<UnaryStringProvider<Calc>>(duplicateParameters());
  };

  bool isConst() const override {
    bool c = true;
    for (const auto &param : params) {
      c &= param->isConst();
    }
    return c;
  }

  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override {
    std::string str;
    if (!getParamString(str, params[0], json)) return RJValue();
    return Calc::calculate(std::move(str), alloc);
  };

  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override {
    DCHECK(!isAtom()) << "Did not check for atom first";
    return nullptr;
  };

  CREATE_FACTORY(UnaryStringProvider<Calc>)

 protected:
  void checkAllParamTypes() {
    for (unsigned int i = 0; i < params.size(); ++i) {
      checkParamType(i, IV_String);
    }
  }
};

/*
 * LEN
 */
struct UnaryLenCalculationFunction {
  static constexpr auto name = "LEN";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(std::string &&val, RJMemoryPoolAlloc &alloc) {
    return RJValue(val.size());
  };
};

typedef UnaryStringProvider<UnaryLenCalculationFunction> LenProvider;

/*
 * UPPER
 */
struct UnaryUpperCalculationFunction {
  static constexpr auto name = "UPPER";
  static constexpr joda::query::IValueType retType = IV_String;

  inline static RJValue calculate(std::string &&val, RJMemoryPoolAlloc &alloc) {
    transform(val.begin(), val.end(), val.begin(), ::toupper);
    return RJValue(val.c_str(), alloc);
  };
};

typedef UnaryStringProvider<UnaryUpperCalculationFunction> UpperProvider;

/*
 * LOWER
 */
struct UnaryLowerCalculationFunction {
  static constexpr auto name = "LOWER";
  static constexpr joda::query::IValueType retType = IV_String;

  inline static RJValue calculate(std::string &&val, RJMemoryPoolAlloc &alloc) {
    transform(val.begin(), val.end(), val.begin(), ::tolower);
    return RJValue(val.c_str(), alloc);
  };
};

typedef UnaryStringProvider<UnaryLowerCalculationFunction> LowerProvider;

/*
 * LTRIM
 */
struct UnaryLtrimCalculationFunction {
  static constexpr auto name = "LTRIM";
  static constexpr joda::query::IValueType retType = IV_String;

  inline static RJValue calculate(std::string &&val, RJMemoryPoolAlloc &alloc) {
    val.erase(val.begin(), std::find_if(val.begin(), val.end(), [](int ch) {
                return !std::isspace(ch);
              }));
    return RJValue(val.c_str(), alloc);
  };
};

typedef UnaryStringProvider<UnaryLtrimCalculationFunction> LtrimProvider;

/*
 * RTRIM
 */
struct UnaryRtrimCalculationFunction {
  static constexpr auto name = "RTRIM";
  static constexpr joda::query::IValueType retType = IV_String;

  inline static RJValue calculate(std::string &&val, RJMemoryPoolAlloc &alloc) {
    val.erase(std::find_if(val.rbegin(), val.rend(),
                           [](int ch) { return !std::isspace(ch); })
                  .base(),
              val.end());
    return RJValue(val.c_str(), alloc);
  };
};

typedef UnaryStringProvider<UnaryRtrimCalculationFunction> RtrimProvider;

template class UnaryStringProvider<UnaryLenCalculationFunction>;

template class UnaryStringProvider<UnaryUpperCalculationFunction>;

template class UnaryStringProvider<UnaryLowerCalculationFunction>;

template class UnaryStringProvider<UnaryLtrimCalculationFunction>;

template class UnaryStringProvider<UnaryRtrimCalculationFunction>;
}  // namespace joda::query
#endif  // JODA_UNARYSTRINGPROVIDER_H
