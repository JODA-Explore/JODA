//
// Created by Nico Schäfer on 11/13/17.
//

#ifndef JODA_COMPAREPREDICATE_H
#define JODA_COMPAREPREDICATE_H

#include <joda/query/values/IValueProvider.h>

#include "Predicate.h"

namespace joda::query {
/**
 * Describes two incompatible types that are supposed to be compared
 */
class NotComparableException : public std::exception {
  virtual const char *what() const throw() {
    return "The given values are not comparable by '<', '<=','>' or '>='";
  }
};

/**
 * Compares two values for larger/smaller (equals) relations
 */
class ComparePredicate : public Predicate {
 public:
  ComparePredicate(std::unique_ptr<IValueProvider> &&lhs,
                   std::unique_ptr<IValueProvider> &&rhs, bool greater = true,
                   bool include = true);
  bool check(const RapidJsonDocument &val) override;
  bool isCompatible(Predicate *other) override;
  std::string getType() override;
  void accept(class PredicateVisitor &v) override;

  std::string toString();
  std::vector<std::string> getAttributes() const;
  std::unique_ptr<Predicate> duplicate() const;
  bool isGreater() const;
  bool isInclude() const;
  std::unique_ptr<IValueProvider> &getLhs();
  std::unique_ptr<IValueProvider> &getRhs();

 protected:
  std::unique_ptr<IValueProvider> lhs;
  std::unique_ptr<IValueProvider> rhs;
  bool greater = true;
  bool include = true;
};
}

#endif  // JODA_COMPAREPREDICATE_H
