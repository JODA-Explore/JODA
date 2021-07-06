//
// Created by Nico on 06/09/2018.
//

#ifndef JODA_BINARYNUMBERPROVIDER_H
#define JODA_BINARYNUMBERPROVIDER_H

#include <cmath>

#include "IValueProvider.h"

#define JODA_MATH_FUNC_NOOP "NOOP"

namespace joda::query {
/**
 * Template class used for all binary mathamatical functions.
 * The template argument has to be a struct with the following
 * attributes/functions.
 * @code{.cpp}
 * struct <StructName> {
 *  static constexpr auto name = "<Name>";
 *  static constexpr auto op = "<OperationSymbol>";
 *  static constexpr IValueType retType = <IValueType returntype>;
 *  inline static RJValue calculate(double lhs, double rhs, RJMemoryPoolAlloc
 * &alloc) { return <result>; }; inline static RJValue calculate(u_int64_t lhs,
 * u_int64_t rhs, RJMemoryPoolAlloc &alloc) {  return <result>;  }; inline
 * static RJValue calculate(int64_t lhs, int64_t rhs, RJMemoryPoolAlloc &alloc)
 * {  return <result>; };
 * };
 * @endcode
 */
template <class Calc>
class BinaryNumberProvider : public joda::query::IValueProvider {
 public:
  explicit BinaryNumberProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters,
      bool opMode = false)
      : IValueProvider(std::move(parameters)), opMode(opMode) {
    checkParamSize(2);
    checkAllParamTypes();
    DCHECK(isAtom()) << "Only atom ReturnTypes allowed";
  };

  joda::query::IValueType getReturnType() const override {
    return Calc::retType;
  }

  std::string getName() const override {
    if (opMode) {
      DCHECK_STRNE(Calc::op, JODA_MATH_FUNC_NOOP) << "No operator assigned";
      return Calc::op;
    } else {
      return Calc::name;
    }
  };

  std::string toString() const override {
    if (!opMode)
      return IValueProvider::toString();
    else {
      DCHECK_STRNE(Calc::op, JODA_MATH_FUNC_NOOP) << "No operator assigned";
      return params[0]->toString() + " " + Calc::op + " " +
             params[1]->toString();
    }
  }

  std::unique_ptr<joda::query::IValueProvider> duplicate() const override {
    return std::make_unique<BinaryNumberProvider<Calc>>(duplicateParameters(),
                                                        opMode);
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
    const RJValue *lhs;
    RJValue tmplhs;
    const RJValue *rhs;
    RJValue tmprhs;
    // Get Pointer to value
    if (params[0]->isAtom()) {
      tmplhs = params[0]->getAtomValue(json, alloc);
      lhs = &tmplhs;
    } else {
      lhs = params[0]->getValue(json, alloc);
    }
    if (lhs == nullptr || !lhs->IsNumber()) return RJValue();
    // Get Pointer to List
    if (params[1]->isAtom()) {
      tmprhs = params[1]->getAtomValue(json, alloc);
      rhs = &tmprhs;
    } else {
      rhs = params[1]->getValue(json, alloc);
    }
    if (rhs == nullptr || !rhs->IsNumber()) return RJValue();

    if (lhs->IsUint64() && rhs->IsUint64())
      return Calc::calculate(lhs->GetUint64(), rhs->GetUint64(), alloc);
    if (lhs->IsInt64() && rhs->IsInt64())
      return Calc::calculate(lhs->GetInt64(), rhs->GetInt64(), alloc);
    return Calc::calculate(lhs->GetDouble(), rhs->GetDouble(), alloc);
  };

  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override {
    DCHECK(!isAtom()) << "Did not check for atom first";
    return nullptr;
  };

  CREATE_FACTORY(BinaryNumberProvider<Calc>)

 protected:
  bool opMode;

  void checkAllParamTypes() {
    for (unsigned int i = 0; i < params.size(); ++i) {
      checkParamType(i, IV_Number);
    }
  }
};

/*
 * Sum (+)
 */
struct BinarySumCalculationFunction {
  static constexpr auto name = "SUM";
  static constexpr auto op = "+";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double lhs, double rhs,
                                  RJMemoryPoolAlloc &alloc) {
    return RJValue(lhs + rhs);
  };

  inline static RJValue calculate(u_int64_t lhs, u_int64_t rhs,
                                  RJMemoryPoolAlloc &alloc) {
    return RJValue(lhs + rhs);
  };

  inline static RJValue calculate(int64_t lhs, int64_t rhs,
                                  RJMemoryPoolAlloc &alloc) {
    return RJValue(lhs + rhs);
  };
};

typedef BinaryNumberProvider<BinarySumCalculationFunction> SumProvider;

/*
 * Sub (-)
 */
struct BinarySubCalculationFunction {
  static constexpr auto name = "SUB";
  static constexpr auto op = "-";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double lhs, double rhs,
                                  RJMemoryPoolAlloc &alloc) {
    return RJValue(lhs - rhs);
  };

  inline static RJValue calculate(u_int64_t lhs, u_int64_t rhs,
                                  RJMemoryPoolAlloc &alloc) {
    if (rhs > lhs) {  // Check for underflow
      return RJValue(int64_t(lhs - rhs));
    }
    return RJValue(lhs - rhs);
  };

  inline static RJValue calculate(int64_t lhs, int64_t rhs,
                                  RJMemoryPoolAlloc &alloc) {
    return RJValue(lhs - rhs);
  };
};

typedef BinaryNumberProvider<BinarySubCalculationFunction> SubtractProvider;

/*
 * Div (/)
 */
struct BinaryDivCalculationFunction {
  static constexpr auto name = "DIV";
  static constexpr auto op = "/";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double lhs, double rhs,
                                  RJMemoryPoolAlloc &alloc) {
    if (rhs == 0) return RJValue();
    return RJValue(lhs / rhs);
  };

  inline static RJValue calculate(u_int64_t lhs, u_int64_t rhs,
                                  RJMemoryPoolAlloc &alloc) {
    if (rhs == 0) return RJValue();
    return RJValue(lhs / rhs);
  };

  inline static RJValue calculate(int64_t lhs, int64_t rhs,
                                  RJMemoryPoolAlloc &alloc) {
    if (rhs == 0) return RJValue();
    return RJValue(lhs / rhs);
  };
};

typedef BinaryNumberProvider<BinaryDivCalculationFunction> DivProvider;

/*
 * Product (/)
 */
struct BinaryProdCalculationFunction {
  static constexpr auto name = "PROD";
  static constexpr auto op = "*";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double lhs, double rhs,
                                  RJMemoryPoolAlloc &alloc) {
    return RJValue(lhs * rhs);
  };

  inline static RJValue calculate(u_int64_t lhs, u_int64_t rhs,
                                  RJMemoryPoolAlloc &alloc) {
    return RJValue(lhs * rhs);
  };

  inline static RJValue calculate(int64_t lhs, int64_t rhs,
                                  RJMemoryPoolAlloc &alloc) {
    return RJValue(lhs * rhs);
  };
};

typedef BinaryNumberProvider<BinaryProdCalculationFunction> ProductProvider;

/*
 * Modulo (%)
 */
struct BinaryModCalculationFunction {
  static constexpr auto name = "MOD";
  static constexpr auto op = "%";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double lhs, double rhs,
                                  RJMemoryPoolAlloc &alloc) {
    if (rhs == 0) return RJValue();
    return RJValue(std::fmod(lhs, rhs));
  };

  inline static RJValue calculate(u_int64_t lhs, u_int64_t rhs,
                                  RJMemoryPoolAlloc &alloc) {
    if (rhs == 0) return RJValue();
    return RJValue(lhs % rhs);
  };

  inline static RJValue calculate(int64_t lhs, int64_t rhs,
                                  RJMemoryPoolAlloc &alloc) {
    if (rhs == 0) return RJValue();
    return RJValue(lhs % rhs);
  };
};

typedef BinaryNumberProvider<BinaryModCalculationFunction> ModuloProvider;

/*
 * Power
 */
struct BinaryPowCalculationFunction {
  static constexpr auto name = "POW";
  static constexpr auto op = JODA_MATH_FUNC_NOOP;
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double lhs, double rhs,
                                  RJMemoryPoolAlloc &alloc) {
    double const res = std::pow(lhs, rhs);
    if (!std::isfinite(res)) return RJValue();
    return RJValue(res);
  };

  inline static RJValue calculate(u_int64_t lhs, u_int64_t rhs,
                                  RJMemoryPoolAlloc &alloc) {
    double const res = std::pow(lhs, rhs);
    if (!std::isfinite(res)) return RJValue();
    return RJValue(res);
  };

  inline static RJValue calculate(int64_t lhs, int64_t rhs,
                                  RJMemoryPoolAlloc &alloc) {
    double const res = std::pow(lhs, rhs);
    if (!std::isfinite(res)) return RJValue();
    return RJValue(res);
  };
};

typedef BinaryNumberProvider<BinaryPowCalculationFunction> PowerProvider;

/*
 * Atan2
 */
struct BinaryAtan2CalculationFunction {
  static constexpr auto name = "ATAN2";
  static constexpr auto op = JODA_MATH_FUNC_NOOP;
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double lhs, double rhs,
                                  RJMemoryPoolAlloc &alloc) {
    return RJValue(std::atan2(lhs, rhs));
  };

  inline static RJValue calculate(u_int64_t lhs, u_int64_t rhs,
                                  RJMemoryPoolAlloc &alloc) {
    return RJValue(std::atan2(lhs, rhs));
  };

  inline static RJValue calculate(int64_t lhs, int64_t rhs,
                                  RJMemoryPoolAlloc &alloc) {
    return RJValue(std::atan2(lhs, rhs));
  };
};

typedef BinaryNumberProvider<BinaryAtan2CalculationFunction> Atan2Provider;

template class BinaryNumberProvider<BinarySumCalculationFunction>;

template class BinaryNumberProvider<BinarySubCalculationFunction>;

template class BinaryNumberProvider<BinaryDivCalculationFunction>;

template class BinaryNumberProvider<BinaryProdCalculationFunction>;

template class BinaryNumberProvider<BinaryModCalculationFunction>;

template class BinaryNumberProvider<BinaryPowCalculationFunction>;

template class BinaryNumberProvider<BinaryAtan2CalculationFunction>;

}  // namespace joda::query
#endif  // JODA_BINARYNUMBERPROVIDER_H
