//
// Created by Nico on 06/09/2018.
//

#ifndef JODA_BINARYSTRINGPROVIDER_H
#define JODA_BINARYSTRINGPROVIDER_H

#include <cmath>

#include "IValueProvider.h"

namespace joda::query {
/**
 * Template class used for all binary string functions.
 * The template argument has to be a struct with the following
 * attributes/functions.
 * @code{.cpp}
 * struct <StructName> {
 *  static constexpr auto name = "<Name>";
 *  static constexpr IValueType retType = <IValueType returntype>;
 *  inline static RJValue calculate(std::string &&lhs, std::string &&rhs, * RJMemoryPoolAlloc &alloc) { return <result>; };
 * };
 * @endcode
 */
template <class Calc>
class BinaryStringProvider : public joda::query::IValueProvider {
 public:
  explicit BinaryStringProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters)
      : IValueProvider(std::move(parameters)) {
    checkParamSize(2);
    checkAllParamTypes();
    DCHECK(isAtom()) << "Only atom ReturnTypes allowed";
  };

  joda::query::IValueType getReturnType() const override { return Calc::retType; }

  std::string getName() const override { return Calc::name; };

  std::string toString() const override { return IValueProvider::toString(); }

  std::unique_ptr<IValueProvider> duplicate() const override {
    return std::make_unique<BinaryStringProvider<Calc>>(duplicateParameters());
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
    std::string lhsStr;
    if (!getParamString(lhsStr, params[0], json)) return RJValue();
    std::string rhsStr;
    if (!getParamString(rhsStr, params[1], json)) return RJValue();

    return Calc::calculate(std::move(lhsStr), std::move(rhsStr), alloc);
  };

  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override {
    DCHECK(!isAtom()) << "Did not check for atom first";
    return nullptr;
  };

  CREATE_FACTORY(BinaryStringProvider<Calc>)

 protected:
  void checkAllParamTypes() {
    for (unsigned int i = 0; i < params.size(); ++i) {
      checkParamType(i, IV_String);
    }
  }
};

/*
 * SCONTAINS
 */
struct BinarySCONTAINSCalculationFunction {
  static constexpr auto name = "SCONTAINS";
  static constexpr joda::query::IValueType retType = IV_Bool;

  inline static RJValue calculate(std::string &&lhs, std::string &&rhs,
                                  RJMemoryPoolAlloc &alloc) {
    return RJValue(lhs.find(rhs) != std::string::npos);
  };
};

typedef BinaryStringProvider<BinarySCONTAINSCalculationFunction>
    SCONTAINSProvider;

/*
 * STARTSWITH
 */
struct BinarySTARTSWITHCalculationFunction {
  static constexpr auto name = "STARTSWITH";
  static constexpr joda::query::IValueType retType = IV_Bool;

  inline static RJValue calculate(std::string &&lhs, std::string &&rhs,
                                  RJMemoryPoolAlloc &alloc) {
    if (rhs.size() > lhs.size()) return RJValue(false);
    return RJValue(lhs.substr(0, rhs.size()) == rhs);
  };
};

typedef BinaryStringProvider<BinarySTARTSWITHCalculationFunction>
    STARTSWITHProvider;

/*
 * CONCAT
 */
struct BinaryConcatCalculationFunction {
  static constexpr auto name = "CONCAT";
  static constexpr joda::query::IValueType retType = IV_String;

  inline static RJValue calculate(std::string &&lhs, std::string &&rhs,
                                  RJMemoryPoolAlloc &alloc) {
    return RJValue((lhs + rhs).c_str(), alloc);
  };
};

typedef BinaryStringProvider<BinaryConcatCalculationFunction> ConcatProvider;

/*
 * FINDSTR
 */
struct BinaryFINDSTRCalculationFunction {
  static constexpr auto name = "FINDSTR";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(std::string &&lhs, std::string &&rhs,
                                  RJMemoryPoolAlloc &alloc) {
    return RJValue((int64_t)lhs.find(rhs));
  };
};

typedef BinaryStringProvider<BinaryFINDSTRCalculationFunction> FINDSTRProvider;


template class BinaryStringProvider<BinarySCONTAINSCalculationFunction>;

template class BinaryStringProvider<BinarySTARTSWITHCalculationFunction>;

template class BinaryStringProvider<BinaryConcatCalculationFunction>;

template class BinaryStringProvider<BinaryFINDSTRCalculationFunction>;
}
#endif  // JODA_BINARYSTRINGPROVIDER_H
