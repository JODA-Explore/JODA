//
// Created by Nico Schäfer on 6/26/17.
//

#ifndef JODA_TOSTRINGVISITOR_H
#define JODA_TOSTRINGVISITOR_H

#include "AndPredicate.h"
#include "ComparePredicate.h"
#include "EqualizePredicate.h"
#include "NegatePredicate.h"
#include "OrPredicate.h"
#include "PredicateVisitor.h"

namespace joda::query {
/**
 * Visitor creating a string representation of the predicates
 */
class ToStringVisitor : public joda::query::PredicateVisitor {
 public:
  void visit(AndPredicate *e) override;
  void visit(NegatePredicate *e) override;
  void visit(OrPredicate *e) override;
  void visit(EqualizePredicate *e) override;
  void visit(ComparePredicate *e) override;
  void visit(ValToPredicate *e) override;

  /**
   * Returns the created string representation and clears the visitor for
   * repeated usage
   * @return
   */
  std::string popString();

  /**
   * Returns the created string representation and keeps the string
   * representation for repeated retrieval
   * @return
   */
  std::string getString();

 private:
  std::string str = "";
};
}  // namespace joda::query

#endif  // JODA_TOSTRINGVISITOR_H
