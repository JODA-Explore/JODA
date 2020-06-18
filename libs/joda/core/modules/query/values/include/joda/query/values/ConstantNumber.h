//
// Created by Nico on 24/05/2019.
//

#ifndef JODA_CONSTANTNUMBER_H
#define JODA_CONSTANTNUMBER_H
#include "AtomProvider.h"
#include "IValueProvider.h"

namespace joda::query {
/**
 * Template class used for all number constants.
 * The template argument has to be a struct with the following
 * attributes/functions.
 * @code{.cpp}
 * struct <StructName> {
 *  typedef <Number Type double, int, ...> AtomType;
 *  static constexpr auto name = "<Name>";
 *  static constexpr auto value() { return <value>; }
 * };
 * @endcode
 */
template<class Calc>
class ConstantNumber : public AtomProvider<typename Calc::AtomType> {
 public:
  explicit ConstantNumber(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters)
      : AtomProvider<typename Calc::AtomType>(Calc::value()) {}

  explicit ConstantNumber()
      : AtomProvider<typename Calc::AtomType>(Calc::value()) {}

  std::string getName() const override { return std::string(Calc::name); }

  std::string toString() const override { return getName() + "()"; }

  CREATE_FACTORY(ConstantNumber<Calc>)
};

/**
 *  PI() (3.14159265359...)
 */
struct PiConstantFunction {
  typedef double AtomType;
  static constexpr auto name = "PI";

  static constexpr auto value() { return M_PI; }
};

typedef ConstantNumber<PiConstantFunction> PiProvider;

/**
 * NOW() (current UNIX timestamp in ms)
 */
struct NowConstantFunction {
  typedef uint64_t AtomType;
  static constexpr auto name = "NOW";

  static auto value() {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    return ms.count();
  }
};

typedef ConstantNumber<NowConstantFunction> NowProvider;

template
class ConstantNumber<joda::query::PiConstantFunction>;

template
class ConstantNumber<joda::query::NowConstantFunction>;
}
#endif  // JODA_CONSTANTNUMBER_H
