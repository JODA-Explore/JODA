//
// Created by Nico Schäfer on 6/26/17.
//

#ifndef JODA_COPYPREDICATEVISITOR_H
#define JODA_COPYPREDICATEVISITOR_H

#include "AndPredicate.h"
#include "ComparePredicate.h"
#include "EqualizePredicate.h"
#include "NegatePredicate.h"
#include "OrPredicate.h"
#include "Predicate.h"
#include "PredicateVisitor.h"

namespace joda::query {
/**
 * Visitor duplicating the whole predicate tree
 */
class CopyPredicateVisitor : public joda::query::PredicateVisitor {
 public:
  void visit(AndPredicate *e) override;
  void visit(NegatePredicate *e) override;
  void visit(OrPredicate *e) override;
  void visit(EqualizePredicate *e) override;
  void visit(ComparePredicate *e) override;
  void visit(ValToPredicate *e) override;

  /**
   * Retrieves the duplicated predicate
   * @return
   */
  std::unique_ptr<Predicate> getPredicate();

 private:
  std::unique_ptr<Predicate> pred;
};
}  // namespace joda::query

#endif  // JODA_COPYPREDICATEVISITOR_H
