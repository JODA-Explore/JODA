//
// Created by "Nico Schaefer" on 12/9/16.
//

#include "../include/joda/query/predicate/AndPredicate.h"

joda::query::AndPredicate::AndPredicate(std::unique_ptr<query::Predicate> p1,
                                        std::unique_ptr<query::Predicate> p2)
    : p1(std::move(p1)), p2(std::move(p2)) {}

bool joda::query::AndPredicate::check(const RapidJsonDocument& val) {
  return p1->check(val) && p2->check(val);
}

bool joda::query::AndPredicate::isCompatible(query::Predicate* other) {
  auto* pred = dynamic_cast<AndPredicate*>(other);
  return pred != nullptr && ((this->p1->isCompatible(pred->p1.get()) &&
                              this->p2->isCompatible(pred->p2.get())) ||
                             (this->p1->isCompatible(pred->p2.get()) &&
                              this->p2->isCompatible(pred->p1.get())));
}

std::string joda::query::AndPredicate::getType() {
  return joda::query::AndPredicate::type;
}

const std::string joda::query::AndPredicate::type = "AndPredicate";

void joda::query::AndPredicate::accept(PredicateVisitor& v) { v.visit(this); }

void joda::query::AndPredicate::subAccept(PredicateVisitor& v, bool lhs) {
  if (lhs) {
    p1->accept(v);
  } else {
    p2->accept(v);
  }
}
