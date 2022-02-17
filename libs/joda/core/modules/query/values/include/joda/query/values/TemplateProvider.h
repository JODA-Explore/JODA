#ifndef JODA_TEMPLATEPROVIDER_H
#define JODA_TEMPLATEPROVIDER_H

#include <joda/misc/RJFwd.h>

#include "IValueProvider.h"
#include "ParameterPack.h"

namespace joda::query {

struct TemplateCalc {
  // Parameters
  typedef values::NoParameter<0> P0;
  typedef values::NoParameter<1> P1;
  typedef values::NoParameter<2> P2;
  typedef values::NoParameter<3> P3;
  typedef values::NoParameter<4> P4;

  // Return value of the function
  static constexpr IValueType retType = IV_Null;

  static constexpr auto name = "PLACEHOLDER";
  /**
   * Calls the check function of all parameters
   * @param parameters the list of passed parameter values
   * @param name the name of the function
   **/
  static void checkParameters(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters){

  };

  static RJValue calculate(
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    return RJValue();
  };
};

/**
 * Template class used for all functions.
 */
template <class Calc>
class TemplateProvider : public joda::query::IValueProvider {
 public:
  explicit TemplateProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters)
      : IValueProvider(std::move(parameters)) {
    Calc::P0::check(params, Calc::name);
    Calc::P1::check(params, Calc::name);
    Calc::P2::check(params, Calc::name);
    Calc::P3::check(params, Calc::name);
    Calc::P4::check(params, Calc::name);
    Calc::checkParameters(state, this->params);
  };

  joda::query::IValueType getReturnType() const override {
    return Calc::retType;
  }

  std::string getName() const override { return Calc::name; };

  std::string toString() const override { return IValueProvider::toString(); }

  std::unique_ptr<IValueProvider> duplicate() const override {
    return std::make_unique<TemplateProvider<Calc>>(duplicateParameters());
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
    try {
      return Calc::calculate(state, params, json, alloc);
    }catch(const values::IncompatibleTypeException& e){
      return RJValue();
    }
  };

  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override {
    DCHECK(!isAtom()) << "Did not check for atom first";
    return nullptr;
  };

  bool isAtom() const override { return true; };

  CREATE_FACTORY(TemplateProvider<Calc>)

 protected:
  typename Calc::State state;
};

}  // namespace joda::query

#endif  // JODA_TEMPLATEPROVIDER_H
