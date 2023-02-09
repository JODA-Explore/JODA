//
// Created by Nico on 06/09/2018.
//

#ifndef JODA_COMPARISON_PROVIDER_H
#define JODA_COMPARISON_PROVIDER_H

#include <cmath>
#include <cstring>

#include "IValueProvider.h"

namespace joda::query {

/**
 * Describes two incompatible types that are supposed to be compared
 */
class NotComparableException : public std::exception {
  public:
  virtual const char *what() const throw() {
    return "The given values are not comparable by '<', '<=','>' or '>='";
  }
};

/**
 * Template class used for all binary comparison functions.
 * The template argument has to be a struct with the following
 * attributes/functions.
 * @code{.cpp}
 * struct <StructName> {
 *  static constexpr auto name = "<Name>";
 *  static constexpr auto symbol = "<Symbol>";
 *  inline static bool calculate(const RJValue* lhs, const RJValue* rhs);
 * };
 * @endcode
 */
template <class Calc>
class ComparisonProvider : public joda::query::IValueProvider {
 public:
  explicit ComparisonProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters)
      : IValueProvider(std::move(parameters)) {
    checkParamSize(2);
    if (!params[0]->comparable() || !params[1]->comparable()) {
      throw joda::query::NotComparableException();
    }
    DCHECK(isAtom()) << "Only atom ReturnTypes allowed";
  };

  joda::query::IValueType getReturnType() const override { return IV_Bool; }

  std::string getName() const override { return Calc::name; };

  std::string toString() const override {
    if (strcmp(Calc::symbol, "") == 0) {
      return IValueProvider::toString();
    }
    return params[0]->toString() + " " + Calc::symbol + " " +
           params[1]->toString();
  }

  std::unique_ptr<IValueProvider> duplicate() const override {
    return std::make_unique<ComparisonProvider<Calc>>(duplicateParameters());
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
    // Get Pointer to value
    if (params[0]->isAtom()) {
      tmplhs = params[0]->getAtomValue(json, alloc);
      lhs = &tmplhs;
    } else {
      lhs = params[0]->getValue(json, alloc);
    }
    if (lhs == nullptr) return RJValue();

    const RJValue *rhs;
    RJValue tmprhs;
    // Get Pointer to value
    if (params[1]->isAtom()) {
      tmprhs = params[1]->getAtomValue(json, alloc);
      rhs = &tmprhs;
    } else {
      rhs = params[1]->getValue(json, alloc);
    }
    if (rhs == nullptr) return RJValue();

    if (lhs->IsString() && rhs->IsString()) {
      std::string lhsStr = lhs->GetString();
      std::string rhsStr = rhs->GetString();
      return RJValue(Calc::calculate(lhsStr, rhsStr));
    }

    if (lhs->IsNumber() && rhs->IsNumber()) {
      double lhsNum = lhs->GetDouble();
      double rhsNum = rhs->GetDouble();
      return RJValue(Calc::calculate(lhsNum, rhsNum));
    }

    return RJValue(false);
  };

  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override {
    DCHECK(!isAtom()) << "Did not check for atom first";
    return nullptr;
  };

  const std::unique_ptr<IValueProvider> &getLhs() const { return params[0]; }
  const std::unique_ptr<IValueProvider> &getRhs() const { return params[1]; }

  CREATE_FACTORY(ComparisonProvider<Calc>)

 protected:
};

/*
 * <
 */
struct LessCalc {
  static constexpr auto name = "LESS";
  static constexpr auto symbol = "<";

  inline static bool calculate(const std::string &lhs, const std::string &rhs) {
    return lhs < rhs;
  };
  inline static bool calculate(const double &lhs, const double &rhs) {
    return lhs < rhs;
  };
};

typedef ComparisonProvider<LessCalc> LessProvider;

/*
 * >
 */
struct GreaterCalc {
  static constexpr auto name = "GREATER";
  static constexpr auto symbol = ">";

  inline static bool calculate(const std::string &lhs, const std::string &rhs) {
    return lhs > rhs;
  };
  inline static bool calculate(const double &lhs, const double &rhs) {
    return lhs > rhs;
  };
};

typedef ComparisonProvider<GreaterCalc> GreaterProvider;

/*
 * <=
 */
struct LessEqualCalc {
  static constexpr auto name = "LESSEQ";
  static constexpr auto symbol = "<=";

  inline static bool calculate(const std::string &lhs, const std::string &rhs) {
    return lhs <= rhs;
  };
  inline static bool calculate(const double &lhs, const double &rhs) {
    return lhs <= rhs;
  };
};

typedef ComparisonProvider<LessEqualCalc> LessEqualProvider;

/*
 * >=
 */
struct GreaterEqualCalc {
  static constexpr auto name = "GREATEREQ";
  static constexpr auto symbol = ">=";

  inline static bool calculate(const std::string &lhs, const std::string &rhs) {
    return lhs >= rhs;
  };
  inline static bool calculate(const double &lhs, const double &rhs) {
    return lhs >= rhs;
  };
};

typedef ComparisonProvider<GreaterEqualCalc> GreaterEqualProvider;

}  // namespace joda::query
#endif  // JODA_COMPARISON_PROVIDER_H
