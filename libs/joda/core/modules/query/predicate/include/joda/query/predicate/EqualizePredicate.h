//
// Created by Nico Schäfer on 11/13/17.
//

#ifndef JODA_EQUALIZEPREDICATE_H
#define JODA_EQUALIZEPREDICATE_H

#include "../../../../../values/include/joda/query/values/IValueProvider.h"
#include "Predicate.h"

namespace joda::query {
/**
 * Describes two incompatible types that are supposed to be checked for
 * incompatability
 */
class NotEqualizableException : public std::exception {
  virtual const char *what() const throw() {
    return "The given values are not comparable by '==' or '!='";
  }
};

/**
 * Prediate checking two calues for equality
 */
class EqualizePredicate : public joda::query::Predicate {
 public:
  EqualizePredicate(std::unique_ptr<IValueProvider> &&lhs,
                    std::unique_ptr<IValueProvider> &&rhs, bool equal);
  bool check(const RapidJsonDocument &val) override;
  bool isCompatible(Predicate *other) override;
  std::string getType() override;
  void accept(class PredicateVisitor &v) override;
  std::string toString();
  std::vector<std::string> getAttributes() const;
  std::unique_ptr<Predicate> duplicate();

  std::unique_ptr<IValueProvider> &getLhs();
  std::unique_ptr<IValueProvider> &getRhs();
  bool isEqual() const;

 protected:
  std::unique_ptr<IValueProvider> lhs;
  std::unique_ptr<IValueProvider> rhs;
  bool equal = true;
};
}  // namespace joda::query

#endif  // JODA_EQUALIZEPREDICATE_H
