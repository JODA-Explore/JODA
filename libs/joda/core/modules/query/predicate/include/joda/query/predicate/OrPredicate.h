//
// Created by Nico Schäfer on 6/26/17.
//

#ifndef JODA_ORPREDICATE_H
#define JODA_ORPREDICATE_H

#include "Predicate.h"
#include "AndPredicate.h"

namespace joda::query {
/**
 * Combines the result of two subpredicates by using the logical OR operation
 */
class OrPredicate : public joda::query::Predicate {
 public:
  explicit OrPredicate(std::unique_ptr<Predicate> p1,
                       std::unique_ptr<Predicate> p2);
  bool check(const RapidJsonDocument &val) override;
  bool isCompatible(Predicate *other) override;
  virtual std::string getType() override;
  void accept(struct PredicateVisitor &v) override;

  const static std::string type;
  void subAccept(struct PredicateVisitor &v, bool lhs);

  friend class CNFPredicateVisitor;
  friend class CNFtoListPredicateVisitor;
  friend class CopyPredicateVisitor;

 protected:
  std::unique_ptr<Predicate> p1, p2;
};
}

#endif  // JODA_ORPREDICATE_H
