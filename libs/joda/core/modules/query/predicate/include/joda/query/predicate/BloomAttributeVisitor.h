//
// Created by Nico Schäfer on 10/31/17.
//

#ifndef JODA_ATTRIBUTEVISITOR_H
#define JODA_ATTRIBUTEVISITOR_H

#include "AndPredicate.h"
#include "ComparePredicate.h"
#include "EqualizePredicate.h"
#include "NegatePredicate.h"
#include "OrPredicate.h"
#include "Predicate.h"
#include "PredicateVisitor.h"
namespace joda::query {
/**
 * Visitor extracting required JSON pointers for usage in Bloom filter
 */
class BloomAttributeVisitor : public joda::query::PredicateVisitor {
 public:
  void visit(AndPredicate *e) override;
  void visit(NegatePredicate *e) override;
  void visit(OrPredicate *e) override;
  void visit(EqualizePredicate *e) override;
  void visit(ComparePredicate *e) override;
  void visit(ValToPredicate *e) override;
  bool isValid() const;

  /**
   * @return the extracted attributes (JSON pointers)
   */
  std::vector<std::string> getAttributes() const;

 protected:
  bool valid = true;
  std::vector<std::string> attributes;
};
}  // namespace joda::query

#endif  // JODA_ATTRIBUTEVISITOR_H
