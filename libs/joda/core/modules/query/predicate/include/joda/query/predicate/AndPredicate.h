//
// Created by "Nico Schaefer" on 12/9/16.
//

#ifndef JODA_ANDPREDICATE_H
#define JODA_ANDPREDICATE_H


#include "Predicate.h"
namespace joda::query {
/**
 * Combines the result of two subpredicates by using the logical AND operation
 */
 class AndPredicate : public Predicate {
 public:
  explicit AndPredicate(std::unique_ptr<Predicate> p1,
                        std::unique_ptr<Predicate> p2);

  bool check(const RapidJsonDocument &val) override;
  bool isCompatible(Predicate *other) override;
  virtual std::string getType() override;
  void accept(PredicateVisitor &v) override;

  /**
   * Accepts either the left hand or right hand predicate
   * @param v The visitor to use
   * @param lhs if true, accepts the left hand predicate, if false accepts the
   * right hand predicate
   */
  void subAccept(PredicateVisitor &v, bool lhs);
  const static std::string type;

  friend class CNFPredicateVisitor;
  friend class CNFtoListPredicateVisitor;
  friend class CopyPredicateVisitor;

 protected:
  std::unique_ptr<Predicate> p1, p2;
};
}



#endif  // JODA_ANDPREDICATE_H
