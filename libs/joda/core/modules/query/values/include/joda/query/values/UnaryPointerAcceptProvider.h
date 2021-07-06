//
// Created by Nico on 15/07/2019.
//

#ifndef JODA_UNARYPOINTERACCEPTPROVIDER_H
#define JODA_UNARYPOINTERACCEPTPROVIDER_H

#include "IValueProvider.h"
#include "PointerProvider.h"
#include "ValueAccepter.h"

namespace joda::query {
/*
 * Concept Calc:
 * class Calc {
 *  typedef <Type> RetType
 *  RJValue Accept(json,alloc,ptr);
 *  RJValue getValue(json,alloc,ptr);
 *  RJValue getValue(json,alloc,ptr);
 *  constexpr name = <name>
 * }
 */

template <class Calc>
class UnaryPointerAcceptProvider : public IValueProvider {
 public:
  explicit UnaryPointerAcceptProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters)
      : IValueProvider(std::move(parameters)) {
    checkParamSize(1);
    if (!Calc::acceptAll) checkParamType(0, Calc::inType);
    DCHECK(isAtom()) << "Only atom ReturnTypes allowed";
    accepter = ValueAccepter(params.front());
  }

  IValueType getReturnType() const override { return Calc::retType; }

  std::string getName() const override { return Calc::name; };

  std::string toString() const override { return IValueProvider::toString(); }

  std::unique_ptr<IValueProvider> duplicate() const override {
    return std::make_unique<UnaryPointerAcceptProvider<Calc>>(
        duplicateParameters());
  };

  bool isAtom() const override { return true; }

  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override {
    DCHECK(!isAtom()) << "Did not check for atom first";
    return nullptr;
  };

  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override {
    DCHECK(isAtom()) << "Did not check for atom first";
    if (params.front()->isAtom()) {
      auto v = params.front()->getAtomValue(json, alloc);
      return Calc::pointer(&v);
    }
    auto ptr = accepter.getPointer(json, alloc);
    if (ptr != nullptr) return Calc::pointer(ptr);
    auto vo = accepter.getObjVO(json, alloc);
    if (vo != nullptr) return Calc::virtualObject(vo);
    return Calc::accept(json, alloc, accepter);
  };

  bool isConst() const override { return params.front()->isConst(); }

  void getAttributes(std::vector<std::string> &vec) const override {}

  CREATE_FACTORY(UnaryPointerAcceptProvider<Calc>)

 private:
  ValueAccepter accepter;
};
}  // namespace joda::query
#endif  // JODA_UNARYPOINTERACCEPTPROVIDER_H
