//
// Created by Nico on 06/09/2018.
//

#ifndef JODA_UNARYNUMBERPROVIDER_H
#define JODA_UNARYNUMBERPROVIDER_H

#include <cmath>
#include "IValueProvider.h"

namespace joda::query {
#define degreesToRadians(angleDegrees) ((angleDegrees)*M_PI / 180.0)
#define radiansToDegrees(angleRadians) ((angleRadians)*180.0 / M_PI)

/**
 * Template class used for all unary mathamatical functions.
 * The template argument has to be a struct with the following
 * attributes/functions.
 * @code{.cpp}
 * struct <StructName> {
 *  static constexpr auto name = "<Name>";
 *  static constexpr IValueType retType = <IValueType returntype>;
 *  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
 * return <result>; }; inline static RJValue calculate(u_int64_t val,
 * RJMemoryPoolAlloc &alloc) {  return <result>;  }; inline static RJValue
 * calculate(int64_t val, RJMemoryPoolAlloc &alloc) {  return <result>; };
 * };
 * @endcode
 */
template <class Calc>
class UnaryNumberProvider : public joda::query::IValueProvider {
 public:
  explicit UnaryNumberProvider(
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

  std::string toString() const override {
    return joda::query::IValueProvider::toString();
  }

  std::unique_ptr<joda::query::IValueProvider> duplicate() const override {
    return std::make_unique<UnaryNumberProvider<Calc>>(duplicateParameters());
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
    if (params[0]->isAtom()) {
      auto val = params[0]->getAtomValue(json, alloc);
      if (!val.IsNumber()) return RJValue();
      if (val.IsUint64()) return Calc::calculate(val.GetUint64(), alloc);
      if (val.IsInt64()) return Calc::calculate(val.GetInt64(), alloc);
      return Calc::calculate(val.GetDouble(), alloc);
    } else {
      auto val = params[0]->getValue(json, alloc);
      if (val == nullptr || !val->IsNumber()) return RJValue();
      if (val->IsUint64()) return Calc::calculate(val->GetUint64(), alloc);
      if (val->IsInt64()) return Calc::calculate(val->GetInt64(), alloc);
      return Calc::calculate(val->GetDouble(), alloc);
    }
  };

  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override {
    DCHECK(!isAtom()) << "Did not check for atom first";
    return nullptr;
  };

  CREATE_FACTORY(UnaryNumberProvider<Calc>)

 protected:
  void checkAllParamTypes() {
    for (unsigned int i = 0; i < params.size(); ++i) {
      checkParamType(i, IV_Number);
    }
  }
};

/*
 * ABS
 */
struct UnaryAbsCalculationFunction {
  static constexpr auto name = "ABS";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::abs(val));
  };

  inline static RJValue calculate(u_int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(val);
  };

  inline static RJValue calculate(int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::abs(val));
  };
};

typedef UnaryNumberProvider<UnaryAbsCalculationFunction> AbsProvider;

/*
 * ROUND
 */
struct UnaryRoundCalculationFunction {
  static constexpr auto name = "ROUND";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::round(val));
  };

  inline static RJValue calculate(u_int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(val);
  };

  inline static RJValue calculate(int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(val);
  };
};

typedef UnaryNumberProvider<UnaryRoundCalculationFunction> RoundProvider;

/*
 * TRUNC
 */
struct UnaryTruncCalculationFunction {
  static constexpr auto name = "TRUNC";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::trunc(val));
  };

  inline static RJValue calculate(u_int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(val);
  };

  inline static RJValue calculate(int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(val);
  };
};

typedef UnaryNumberProvider<UnaryTruncCalculationFunction> TruncProvider;

/*
 * SQRT
 */
struct UnarySqrtCalculationFunction {
  static constexpr auto name = "SQRT";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
    if (val < 0) return RJValue();
    return RJValue(std::sqrt(val));
  };

  inline static RJValue calculate(u_int64_t val, RJMemoryPoolAlloc &alloc) {
    if (val < 0) return RJValue();
    return RJValue(std::sqrt(val));
  };

  inline static RJValue calculate(int64_t val, RJMemoryPoolAlloc &alloc) {
    if (val < 0) return RJValue();
    return RJValue(std::sqrt(val));
  };
};

typedef UnaryNumberProvider<UnarySqrtCalculationFunction> SqrtProvider;

/*
 * CEIL
 */
struct UnaryCeilCalculationFunction {
  static constexpr auto name = "CEIL";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::ceil(val));
  };

  inline static RJValue calculate(u_int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(val);
  };

  inline static RJValue calculate(int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(val);
  };
};

typedef UnaryNumberProvider<UnaryCeilCalculationFunction> CeilProvider;

/*
 * FLOOR
 */
struct UnaryFloorCalculationFunction {
  static constexpr auto name = "FLOOR";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::floor(val));
  };

  inline static RJValue calculate(u_int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(val);
  };

  inline static RJValue calculate(int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(val);
  };
};

typedef UnaryNumberProvider<UnaryFloorCalculationFunction> FloorProvider;

/*
 * DEGREES
 */
struct UnaryDegreesCalculationFunction {
  static constexpr auto name = "DEGREES";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
    return RJValue(radiansToDegrees(val));
  };

  inline static RJValue calculate(u_int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(radiansToDegrees(val));
  };

  inline static RJValue calculate(int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(radiansToDegrees(val));
  };
};

typedef UnaryNumberProvider<UnaryDegreesCalculationFunction> DegreesProvider;

/*
 * RADIANS
 */
struct UnaryRadiansCalculationFunction {
  static constexpr auto name = "RADIANS";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
    return RJValue(degreesToRadians(val));
  };

  inline static RJValue calculate(u_int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(degreesToRadians(val));
  };

  inline static RJValue calculate(int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(degreesToRadians(val));
  };
};

typedef UnaryNumberProvider<UnaryRadiansCalculationFunction> RadiansProvider;

/*
 * ACOS
 */
struct UnaryAcosCalculationFunction {
  static constexpr auto name = "ACOS";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
    if (val < -1 || val > 1) return RJValue();
    return RJValue(std::acos(val));
  };

  inline static RJValue calculate(u_int64_t val, RJMemoryPoolAlloc &alloc) {
    if (val > 1) return RJValue();
    return RJValue(std::acos(val));
  };

  inline static RJValue calculate(int64_t val, RJMemoryPoolAlloc &alloc) {
    if (val < -1 || val > 1) return RJValue();
    return RJValue(std::acos(val));
  };
};

typedef UnaryNumberProvider<UnaryAcosCalculationFunction> AcosProvider;

/*
 * ASIN
 */
struct UnaryAsinCalculationFunction {
  static constexpr auto name = "ASIN";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
    if (val < -1 || val > 1) return RJValue();
    return RJValue(std::asin(val));
  };

  inline static RJValue calculate(u_int64_t val, RJMemoryPoolAlloc &alloc) {
    if (val > 1) return RJValue();
    return RJValue(std::asin(val));
  };

  inline static RJValue calculate(int64_t val, RJMemoryPoolAlloc &alloc) {
    if (val < -1 || val > 1) return RJValue();
    return RJValue(std::asin(val));
  };
};

typedef UnaryNumberProvider<UnaryAsinCalculationFunction> AsinProvider;

/*
 * ATAN
 */
struct UnaryAtanCalculationFunction {
  static constexpr auto name = "ATAN";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::atan(val));
  };

  inline static RJValue calculate(u_int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::atan(val));
  };

  inline static RJValue calculate(int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::atan(val));
  };
};

typedef UnaryNumberProvider<UnaryAtanCalculationFunction> AtanProvider;

/*
 * COS
 */
struct UnaryCosCalculationFunction {
  static constexpr auto name = "COS";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::cos(val));
  };

  inline static RJValue calculate(u_int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::cos(val));
  };

  inline static RJValue calculate(int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::cos(val));
  };
};

typedef UnaryNumberProvider<UnaryCosCalculationFunction> CosProvider;

/*
 * SIN
 */
struct UnarySinCalculationFunction {
  static constexpr auto name = "SIN";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::sin(val));
  };

  inline static RJValue calculate(u_int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::sin(val));
  };

  inline static RJValue calculate(int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::sin(val));
  };
};

typedef UnaryNumberProvider<UnarySinCalculationFunction> SinProvider;

/*
 * TAN
 */
struct UnaryTanCalculationFunction {
  static constexpr auto name = "TAN";
  static constexpr joda::query::IValueType retType = IV_Number;

  inline static RJValue calculate(double val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::tan(val));
  };

  inline static RJValue calculate(u_int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::tan(val));
  };

  inline static RJValue calculate(int64_t val, RJMemoryPoolAlloc &alloc) {
    return RJValue(std::tan(val));
  };
};

typedef UnaryNumberProvider<UnaryTanCalculationFunction> TanProvider;

template class UnaryNumberProvider<UnaryAbsCalculationFunction>;

template class UnaryNumberProvider<UnaryRoundCalculationFunction>;

template class UnaryNumberProvider<UnaryTruncCalculationFunction>;

template class UnaryNumberProvider<UnarySqrtCalculationFunction>;

template class UnaryNumberProvider<UnaryCeilCalculationFunction>;

template class UnaryNumberProvider<UnaryFloorCalculationFunction>;

template class UnaryNumberProvider<UnaryDegreesCalculationFunction>;

template class UnaryNumberProvider<UnaryRadiansCalculationFunction>;

template class UnaryNumberProvider<UnaryAcosCalculationFunction>;

template class UnaryNumberProvider<UnaryAsinCalculationFunction>;

template class UnaryNumberProvider<UnaryAtanCalculationFunction>;

template class UnaryNumberProvider<UnaryCosCalculationFunction>;

template class UnaryNumberProvider<UnarySinCalculationFunction>;

template class UnaryNumberProvider<UnaryTanCalculationFunction>;
}  // namespace joda::query
#endif  // JODA_UNARYNUMBERPROVIDER_H
