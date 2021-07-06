//
// Created by Nico Schäfer on 6/26/17.
//

#ifndef JODA_PREDICATEVISITOR_H
#define JODA_PREDICATEVISITOR_H

#include <cassert>
#include <memory>

namespace joda::query {
class AndPredicate;

class NegatePredicate;

class OrPredicate;

class EqualizePredicate;

class ComparePredicate;

class Predicate;

class ValToPredicate;

/**
 * Interface for visitors on predicate trees
 */
class PredicateVisitor {
 public:
  /**
   *  Visits an AndPredicate
   * @param e Predicate to visit
   */
  virtual void visit(AndPredicate* e) = 0;

  /**
   *  Visits an NegatePredicate
   * @param e Predicate to visit
   */
  virtual void visit(NegatePredicate* e) = 0;

  /**
   *  Visits an OrPredicate
   * @param e Predicate to visit
   */
  virtual void visit(OrPredicate* e) = 0;

  /**
   *  Visits an EqualizePredicate
   * @param e Predicate to visit
   */
  virtual void visit(EqualizePredicate* e) = 0;

  /**
   *  Visits an ComparePredicate
   * @param e Predicate to visit
   */
  virtual void visit(ComparePredicate* e) = 0;

  /**
   *  Visits an ValToPredicate
   * @param e Predicate to visit
   */
  virtual void visit(ValToPredicate* e) = 0;
};
}  // namespace joda::query

#endif  // JODA_PREDICATEVISITOR_H
