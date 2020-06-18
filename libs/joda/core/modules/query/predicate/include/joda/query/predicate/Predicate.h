//
// Created by "Nico Schaefer" on 11/25/16.
//

#ifndef JODA_PREDICATE_H
#define JODA_PREDICATE_H
namespace joda::query {
class PredicateVisitor;
}
#include <memory>

#include "PredicateVisitor.h"
#include "joda/document/RapidJsonDocument.h"

namespace joda::query {
/**
 * Predicates represent the filtering CHOOSE predicate.
 * They are used to decide if a given document is in the result set or not.
 */
class Predicate {
 public:
  virtual ~Predicate() = default;

  /**
   * Checks if a given document fulfills the given predicate
   * @param val the document to check
   * @return true if the document fulfills the predicate, else false.
   */
  virtual bool check(const RapidJsonDocument &val) = 0;

  /**
   * Checks if two predicates are semantically equivalent
   * @return true if they are equivalent, false if not
   */
  virtual bool isCompatible(Predicate *other);

  /**
   * Returns the classname of the predicate.
   * @return
   */
  virtual std::string getType();

  /**
   * Uses the given PredicateVisitor to traverse the predicate tree with the
   * visitor pattern.
   * @param v the visitor to use.
   */
  virtual void accept(class PredicateVisitor &v) = 0;

  const static std::string type;
};

typedef std::shared_ptr<Predicate> PredType;
}


#endif  // JODA_PREDICATE_H
