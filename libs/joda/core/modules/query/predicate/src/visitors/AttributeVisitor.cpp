//
// Created by nico on 10/31/17.
//

#include <sstream>
#include "../../include/joda/query/predicate/AttributeVisitor.h"
#include "../../include/joda/query/predicate/EqualizePredicate.h"
#include "../../include/joda/query/predicate/AndPredicate.h"
#include "../../include/joda/query/predicate/NegatePredicate.h"
#include "../../include/joda/query/predicate/OrPredicate.h"
#include "../../include/joda/query/predicate/ComparePredicate.h"
#include "../../include/joda/query/predicate/ValToPredicate.h"

void joda::query::AttributeVisitor::visit(AndPredicate *e) {
  e->subAccept(*this, true);
  e->subAccept(*this, false);
}

void joda::query::AttributeVisitor::visit(NegatePredicate *e) {
  e->subAccept(*this);
}

void joda::query::AttributeVisitor::visit(OrPredicate *e) {
  e->subAccept(*this, true);
  e->subAccept(*this, false);
}

std::vector<std::string> joda::query::AttributeVisitor::getAttributes() const {
  return attributes;
}

void joda::query::AttributeVisitor::visit(EqualizePredicate *e) {
  e->getLhs()->getAttributes(attributes);
  e->getRhs()->getAttributes(attributes);
}

void joda::query::AttributeVisitor::visit(ComparePredicate *e) {
  e->getLhs()->getAttributes(attributes);
  e->getRhs()->getAttributes(attributes);
}

void joda::query::AttributeVisitor::visit(ValToPredicate *e) {
  e->getAttributes(attributes);
}

