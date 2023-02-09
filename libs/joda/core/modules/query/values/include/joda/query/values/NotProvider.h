//
// Created by Nico on 06/09/2018.
//

#ifndef JODA_NOTPROVIDER_H
#define JODA_NOTPROVIDER_H

#include <joda/misc/RJFwd.h>
#include "IValueProvider.h"

namespace joda::query {

class NotProvider : public joda::query::IValueProvider {
 public:
  explicit NotProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters)
      : IValueProvider(std::move(parameters)) {
    checkParamSize(1);
    checkParamType(0,IV_Bool);
    DCHECK(isAtom()) << "Only atom ReturnTypes allowed";
  };

  joda::query::IValueType getReturnType() const override {
    return IV_Bool;
  }

  std::string getName() const override { return "NOT"; };

  std::string toString() const override { return "!" + params[0]->toString(); }

  std::unique_ptr<IValueProvider> duplicate() const override {
    return std::make_unique<NotProvider>(duplicateParameters());
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
    if (lhs == nullptr || !lhs->IsBool()) return RJValue();
    auto lhsBool = lhs->GetBool();
    return RJValue(!lhsBool);
  };

  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override {
    DCHECK(!isAtom()) << "Did not check for atom first";
    return nullptr;
  };

   virtual std::unique_ptr<IValueProvider> optimize() override {
    IValueProvider::optimize(); // Optimize params first

    auto subnot = dynamic_cast<NotProvider*>(params[0].get());
    if(subnot != nullptr){
      // NOT(NOT(x)) = x
      return subnot->params[0]->duplicate();
    }
    
    return nullptr;
  }

  const std::unique_ptr<IValueProvider> &getSub() const { return params[0]; }

  CREATE_FACTORY(NotProvider)

 protected:

};

}  // namespace joda::query
#endif  // JODA_NOTPROVIDER_H
