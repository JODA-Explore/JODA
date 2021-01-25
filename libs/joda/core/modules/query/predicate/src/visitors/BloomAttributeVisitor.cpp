//
// Created by Nico Schäfer on 10/31/17.
//

#include <sstream>
#include "../../include/joda/query/predicate/BloomAttributeVisitor.h"
#include "../../include/joda/query/predicate/EqualizePredicate.h"
#include "../../include/joda/query/predicate/ComparePredicate.h"
#include "../../include/joda/query/predicate/ValToPredicate.h"


void joda::query::BloomAttributeVisitor::visit(joda::query::AndPredicate *e) {
  e->subAccept(*this, true);
  e->subAccept(*this, false);
}
void joda::query::BloomAttributeVisitor::visit(NegatePredicate *e) {
  valid = false;
}

void joda::query::BloomAttributeVisitor::visit(OrPredicate *e) {
  valid = false;
}

bool joda::query::BloomAttributeVisitor::isValid() const {
  return valid;
}
std::vector<std::string> joda::query::BloomAttributeVisitor::getAttributes() const {
  if (valid) return attributes;
  else return std::vector<std::string>();
}
void joda::query::BloomAttributeVisitor::visit(EqualizePredicate *e) {
  e->getLhs()->getAttributes(attributes);
  e->getRhs()->getAttributes(attributes);
}
void joda::query::BloomAttributeVisitor::visit(ComparePredicate *e) {
  e->getLhs()->getAttributes(attributes);
  e->getRhs()->getAttributes(attributes);
}

void joda::query::BloomAttributeVisitor::visit(ValToPredicate *e) {
  e->getAttributes(attributes);
}

