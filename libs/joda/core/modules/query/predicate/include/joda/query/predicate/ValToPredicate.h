//
// Created by Nico Schäfer on 11/23/17.
//

#ifndef JODA_VALTOPREDICATE_H
#define JODA_VALTOPREDICATE_H

#include "../../../../../values/include/joda/query/values/IValueProvider.h"
#include "Predicate.h"

namespace joda::query {
/**
 * Predicate converting a IValueProvider value into a boolean value.
 * If the value is boolean, the result is used directly
 * Else, false is returned
 */
class ValToPredicate : public joda::query::Predicate {
 public:
  explicit ValToPredicate(std::unique_ptr<IValueProvider> &&val);
  explicit ValToPredicate(bool b);
  bool check(const RapidJsonDocument &val) override;
  bool isCompatible(Predicate *other) override;
  std::string getType() override;
  void accept(class PredicateVisitor &v) override;

  std::unique_ptr<ValToPredicate> duplicate() const;
  std::unique_ptr<IValueProvider> duplicateIVal() const;
  std::string toString() const;

  /**
   * Checks if the value evaluates to a constant true value
   * @return true if yes, false else
   */
  bool isConstTrue() const;
  /**
   * Checks if the value evaluates to a constant false value
   * @return true if yes, false else
   */
  bool isConstFalse() const;

  /**
   * Fills the given vector with attributes used in the IValueProvider
   * @param vec the vector to fill
   */
  void getAttributes(std::vector<std::string> &vec) const;
  static const std::string type;

 protected:
  std::unique_ptr<IValueProvider> val;
};
}  // namespace joda::query

#endif  // JODA_VALTOPREDICATE_H
