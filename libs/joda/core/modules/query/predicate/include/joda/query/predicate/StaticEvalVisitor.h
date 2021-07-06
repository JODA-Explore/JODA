//
// Created by Nico Schäfer on 12/13/17.
//

#ifndef JODA_STATICEVALVISITOR_H
#define JODA_STATICEVALVISITOR_H

#include "AndPredicate.h"
#include "ComparePredicate.h"
#include "EqualizePredicate.h"
#include "NegatePredicate.h"
#include "OrPredicate.h"
#include "PredicateVisitor.h"

namespace joda::query {
/**
 * Visitor analyzing a predicate tree for sub-trees with constant/static
 * results. If such a subtree is found, it is replaced by the constant result in
 * a copied instance.
 */
class StaticEvalVisitor : public joda::query::PredicateVisitor {
 public:
  void visit(AndPredicate *e) override;
  void visit(NegatePredicate *e) override;
  void visit(OrPredicate *e) override;
  void visit(EqualizePredicate *e) override;
  void visit(ComparePredicate *e) override;
  void visit(ValToPredicate *e) override;

  /**
   * @return the optimized predicate
   */
  std::unique_ptr<Predicate> getPred();

 protected:
  bool changed = false;
  std::unique_ptr<Predicate> pred;
};
}  // namespace joda::query

#endif  // JODA_STATICEVALVISITOR_H
