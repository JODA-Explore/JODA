//
// Created by Nico on 06/09/2018.
//

#ifndef JODA_BinaryBoolProvider_H
#define JODA_BinaryBoolProvider_H

#include <cmath>

#include "IValueProvider.h"
#include "AtomProvider.h"
#include "NotProvider.h"

namespace joda::query {
/**
 * Template class used for all binary bool functions.
 * The template argument has to be a struct with the following
 * attributes/functions.
 * @code{.cpp}
 * struct <StructName> {
 *  static constexpr auto name = "<Name>";
 *  static constexpr auto symbol = "<Symbol>";
 *  inline static bool calculate(std::bool &lhs, std::bool &rhs) { return
 * <result>; };
 * };
 * @endcode
 */
template <class Calc>
class BinaryBoolProvider : public joda::query::IValueProvider {
 public:
  explicit BinaryBoolProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters)
      : IValueProvider(std::move(parameters)) {
    checkParamSize(2);
    checkAllParamTypes();
    DCHECK(isAtom()) << "Only atom ReturnTypes allowed";
  };

  joda::query::IValueType getReturnType() const override {
    return IV_Bool;
  }

  std::string getName() const override { return Calc::name; };

  std::string toString() const override {
    if (strcmp(Calc::symbol,"") == 0) {
      return IValueProvider::toString();
    }
    return "(" + params[0]->toString() + " " + Calc::symbol + " " +
           params[1]->toString() + ")";
  }

  std::unique_ptr<IValueProvider> duplicate() const override {
    return std::make_unique<BinaryBoolProvider<Calc>>(duplicateParameters());
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
    bool lhsBool = false;
    if (lhs != nullptr && lhs->IsBool()){ 
      lhsBool = lhs->GetBool();
    }

    const RJValue *rhs;
    RJValue tmprhs;
    // Get Pointer to value
    if (params[1]->isAtom()) {
      tmprhs = params[1]->getAtomValue(json, alloc);
      rhs = &tmprhs;
    } else {
      rhs = params[1]->getValue(json, alloc);
    }
    bool rhsBool = false;
    if (rhs != nullptr && rhs->IsBool()) {
      rhsBool = rhs->GetBool();
    }

    return RJValue(Calc::calculate(lhsBool, rhsBool));
  };

  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override {
    DCHECK(!isAtom()) << "Did not check for atom first";
    return nullptr;
  };

  virtual std::unique_ptr<IValueProvider> optimize() override {
    IValueProvider::optimize(); // Optimize params first
    
    if(params[0]->isConst() && params[0]->isBool() && params[0]->isAtom()) {
      RapidJsonDocument doc;
      RJMemoryPoolAlloc alloc;
      auto lhs = params[0]->getAtomValue(doc,alloc).GetBool();
      return Calc::optimize(lhs, params[1]);
    }
    if(params[1]->isConst() && params[1]->isBool()&& params[1]->isAtom()) {
      RapidJsonDocument doc;
      RJMemoryPoolAlloc alloc;
      auto rhs = params[1]->getAtomValue(doc,alloc).GetBool();
      return Calc::optimize(params[0],rhs );
    }
    return nullptr;
  }

  const std::unique_ptr<IValueProvider> &getLhs() const { return params[0]; }
  const std::unique_ptr<IValueProvider> &getRhs() const { return params[1]; }

  CREATE_FACTORY(BinaryBoolProvider<Calc>)

 protected:
  void checkAllParamTypes() {
    for (unsigned int i = 0; i < params.size(); ++i) {
      checkParamType(i, IV_Bool);
    }
  }
};

/*
 * AND
 */
struct AndCalc {
  static constexpr auto name = "AND";
  static constexpr auto symbol = "&&";

  inline static bool calculate(bool lhs, bool rhs) { return lhs && rhs; };

  inline static std::unique_ptr<IValueProvider> optimize(bool lhs, const std::unique_ptr<IValueProvider>& rhs) {
    if (lhs) { // T && Y = Y
      return rhs->duplicate();
    }
    // F && Y = F
    return std::make_unique<BoolProvider>(false);
  };

  inline static std::unique_ptr<IValueProvider> optimize(const std::unique_ptr<IValueProvider>& lhs, bool rhs) { 
    if (rhs) {
      // X && T = X
      return lhs->duplicate();
    }
    // X && F = F
    return std::make_unique<BoolProvider>(false);
  };
};

typedef BinaryBoolProvider<AndCalc> AndProvider;

/*
 * OR
 */
struct OrCalc {
  static constexpr auto name = "OR";
  static constexpr auto symbol = "||";

  inline static bool calculate(bool lhs, bool rhs) { return lhs || rhs; };

  inline static std::unique_ptr<IValueProvider> optimize(bool lhs, const std::unique_ptr<IValueProvider>& rhs) {
  
    if (lhs) { // T || Y = T
      return std::make_unique<BoolProvider>(true);
    }
    // F || Y = Y
    return rhs->duplicate();
  };

  inline static std::unique_ptr<IValueProvider> optimize(const std::unique_ptr<IValueProvider>& lhs, bool rhs) { 
    if (rhs) { // X || T = T
      return std::make_unique<BoolProvider>(true);
    }
    // X || F = X
    return lhs->duplicate();
  };

};

typedef BinaryBoolProvider<OrCalc> OrProvider;

/*
 * XOR
 */
struct XorCalc {
  static constexpr auto name = "XOR";
  static constexpr auto symbol = "";

  inline static bool calculate(bool lhs, bool rhs) {
    return (lhs || rhs) && !(lhs && rhs);
  };

  inline static std::unique_ptr<IValueProvider> optimize(bool lhs, const std::unique_ptr<IValueProvider>& rhs) {
    if (lhs) {
      // T ^ Y = !Y
      std::vector<std::unique_ptr<IValueProvider>> params;
      params.emplace_back(rhs->duplicate());
      return std::make_unique<NotProvider>(std::move(params));
    }
    // F ^ Y = Y
    return rhs->duplicate();
  };

  inline static std::unique_ptr<IValueProvider> optimize(const std::unique_ptr<IValueProvider>& lhs, bool rhs) { 
    if (rhs) {
      // X ^ T = !X
      std::vector<std::unique_ptr<IValueProvider>> params;
    params.emplace_back(lhs->duplicate());
    return std::make_unique<NotProvider>(std::move(params));
    }
    // X ^ F = X
    return lhs->duplicate();
  };

};

typedef BinaryBoolProvider<XorCalc> XorProvider;

/*
 * IMPLICATION
 */
struct ImplicationCalc {
  static constexpr auto name = "IMPLICATION";
  static constexpr auto symbol = "";

  inline static bool calculate(bool lhs, bool rhs) {
    return !lhs || rhs;
  };

  inline static std::unique_ptr<IValueProvider> optimize(bool lhs, const std::unique_ptr<IValueProvider>& rhs) {
    if (lhs) {
      // T -> Y = Y
      return rhs->duplicate();
    }
    // F -> Y = T
    return std::make_unique<BoolProvider>(true);
  };

  inline static std::unique_ptr<IValueProvider> optimize(const std::unique_ptr<IValueProvider>& lhs, bool rhs) { 
    if (rhs) {
      // X -> T = T
      return std::make_unique<BoolProvider>(true);
    }
    // X -> F = !X
    std::vector<std::unique_ptr<IValueProvider>> params;
    params.emplace_back(lhs->duplicate());
    return std::make_unique<NotProvider>(std::move(params));
  };

};

typedef BinaryBoolProvider<ImplicationCalc> ImplicationProvider;

template class BinaryBoolProvider<AndCalc>;
template class BinaryBoolProvider<OrCalc>;
template class BinaryBoolProvider<XorCalc>;
template class BinaryBoolProvider<ImplicationCalc>;
}  // namespace joda::query
#endif  // JODA_BinaryBoolProvider_H
