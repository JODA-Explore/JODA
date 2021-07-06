//
// Created by nico on 10/31/17.
//

#ifndef PJDB_ATTRIBUTEVISITOR_H
#define PJDB_ATTRIBUTEVISITOR_H

#include "AndPredicate.h"
#include "PredicateVisitor.h"
namespace joda::query {
class AttributeVisitor : public PredicateVisitor {
 public:
  void visit(AndPredicate *e) override;
  void visit(NegatePredicate *e) override;
  void visit(OrPredicate *e) override;
  void visit(EqualizePredicate *e) override;
  void visit(ComparePredicate *e) override;
  void visit(ValToPredicate *e) override;
  std::vector<std::string> getAttributes() const;

 protected:
  std::vector<std::string> attributes;
};
}  // namespace joda::query

#endif  // PJDB_ATTRIBUTEVISITOR_H
