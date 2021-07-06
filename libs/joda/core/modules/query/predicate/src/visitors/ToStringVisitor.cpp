//
// Created by Nico Schäfer on 6/26/17.
//

#include "../../include/joda/query/predicate/ToStringVisitor.h"
#include "../../include/joda/query/predicate/AndPredicate.h"
#include "../../include/joda/query/predicate/ComparePredicate.h"
#include "../../include/joda/query/predicate/EqualizePredicate.h"
#include "../../include/joda/query/predicate/OrPredicate.h"
#include "../../include/joda/query/predicate/ValToPredicate.h"
#include "joda/query/predicate/NegatePredicate.h"

std::string joda::query::ToStringVisitor::popString() {
  auto tmp = str;
  str = "";
  return tmp;
}

std::string joda::query::ToStringVisitor::getString() { return str; }

void joda::query::ToStringVisitor::visit(joda::query::AndPredicate* e) {
  str += "(";
  e->subAccept(*this, true);
  str += " && ";
  e->subAccept(*this, false);
  str += ")";
}

void joda::query::ToStringVisitor::visit(joda::query::NegatePredicate* e) {
  str += "!";
  e->subAccept(*this);
}

void joda::query::ToStringVisitor::visit(joda::query::OrPredicate* e) {
  str += "(";
  e->subAccept(*this, true);
  str += " || ";
  e->subAccept(*this, false);
  str += ")";
}

void joda::query::ToStringVisitor::visit(joda::query::EqualizePredicate* e) {
  str += e->toString();
}
void joda::query::ToStringVisitor::visit(joda::query::ComparePredicate* e) {
  str += e->toString();
}

void joda::query::ToStringVisitor::visit(joda::query::ValToPredicate* e) {
  str += e->toString();
}
