//
// Created by "Nico Schaefer" on 12/9/16.
//

#ifndef JODA_NEGATEPREDICATE_H
#define JODA_NEGATEPREDICATE_H

#include "Predicate.h"
#include "AndPredicate.h"

namespace joda::query {
/**
 * Negates the result of the subpredicate
 */
class NegatePredicate : public joda::query::Predicate {
 public:
  explicit NegatePredicate(std::unique_ptr<Predicate> pred);
  bool check(const RapidJsonDocument &val) override;
  bool isCompatible(Predicate *other) override;
  virtual std::string getType() override;
  void accept(struct PredicateVisitor &v) override;

  void setPredicate(std::unique_ptr<Predicate> pred);
  void subAccept(struct PredicateVisitor &v);
  const static std::string type;

  friend class CNFPredicateVisitor;
  friend class CNFtoListPredicateVisitor;
  friend class CopyPredicateVisitor;
  friend class CrackPredicateVisitor;

 protected:
  std::unique_ptr<Predicate> pred;
};
}

#endif  // JODA_NEGATEPREDICATE_H
