//
// Created by Nico on 06/09/2018.
//

#ifndef JODA_EqualityProvider_H
#define JODA_EqualityProvider_H

#include <cmath>

#include "IValueProvider.h"

namespace joda::query {
class NotEqualizableException : public std::exception {
  public:
  virtual const char *what() const throw() {
    return "The given values are not comparable by '==' or '!='";
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
class EqualityProvider : public joda::query::IValueProvider {
 public:
  explicit EqualityProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters)
      : IValueProvider(std::move(parameters)) {
    checkParamSize(2);
    if (!params[0]->equalizable() || !params[1]->equalizable()) {
      throw joda::query::NotEqualizableException();
    }
    DCHECK(isAtom()) << "Only atom ReturnTypes allowed";
  };

  joda::query::IValueType getReturnType() const override { return IV_Bool; }

  std::string getName() const override { return Calc::name; };

  std::string toString() const override {
    if (strcmp(Calc::symbol,"") == 0) {
      return IValueProvider::toString();
    }
    return params[0]->toString() + " " + Calc::symbol + " " +
           params[1]->toString();
  }

  std::unique_ptr<IValueProvider> duplicate() const override {
    return std::make_unique<EqualityProvider<Calc>>(duplicateParameters());
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
    return RJValue(Calc::calculate(params[0], params[1], json));
  };

  RJValue getAtomValue(const RapidJsonDocument &lhs,
                       const RapidJsonDocument &rhs,
                       RJMemoryPoolAlloc &alloc) const {
    return RJValue(Calc::calculate(params[0], params[1], lhs, rhs));
  };

  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override {
    DCHECK(!isAtom()) << "Did not check for atom first";
    return nullptr;
  };

  const std::unique_ptr<IValueProvider> &getLhs() const { return params[0]; }
  const std::unique_ptr<IValueProvider> &getRhs() const { return params[1]; }

  CREATE_FACTORY(EqualityProvider<Calc>)

 protected:
};

/*
 * ==
 */
struct EqualCalc {
  static constexpr auto name = "EQUAL";
  static constexpr auto symbol = "==";

  inline static bool calculate(const std::unique_ptr<IValueProvider> &lhs,
                               const std::unique_ptr<IValueProvider> &rhs,
                               const RapidJsonDocument &json) {
    auto ret = lhs->equal(rhs.get(), json);
    return ret.value_or(false);
  };

  inline static bool calculate(const std::unique_ptr<IValueProvider> &lhs,
                               const std::unique_ptr<IValueProvider> &rhs,
                               const RapidJsonDocument &lhsDoc,
                               const RapidJsonDocument &rhsDoc) {
    auto ret = lhs->equal(rhs.get(), lhsDoc, rhsDoc);
    return ret.value_or(false);
  };
};

typedef EqualityProvider<EqualCalc> EqualProvider;

/*
 * !=
 */
struct UnEqualCalc {
  static constexpr auto name = "UNEQUAL";
  static constexpr auto symbol = "!=";

  inline static bool calculate(const std::unique_ptr<IValueProvider> &lhs,
                               const std::unique_ptr<IValueProvider> &rhs,
                               const RapidJsonDocument &json) {
    auto ret = lhs->equal(rhs.get(), json);
    if (ret.has_value()) {
      return !ret.value();
    }
    return false;
  };
  inline static bool calculate(const std::unique_ptr<IValueProvider> &lhs,
                               const std::unique_ptr<IValueProvider> &rhs,
                               const RapidJsonDocument &lhsDoc,
                               const RapidJsonDocument &rhsDoc) {
    auto ret = lhs->equal(rhs.get(), lhsDoc, rhsDoc);
    if (ret.has_value()) {
      return !ret.value();
    }
    return false;
  };
};

typedef EqualityProvider<UnEqualCalc> UnequalProvider;

}  // namespace joda::query
#endif  // JODA_EqualityProvider_H
