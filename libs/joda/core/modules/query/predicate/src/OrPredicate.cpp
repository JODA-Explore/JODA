//
// Created by Nico Schäfer on 6/26/17.
//

#include "../include/joda/query/predicate/OrPredicate.h"

joda::query::OrPredicate::OrPredicate(std::unique_ptr<Predicate> p1,
                                      std::unique_ptr<Predicate> p2)
    : p1(std::move(p1)), p2(std::move(p2)) {}

bool joda::query::OrPredicate::check(const RapidJsonDocument& val) {
  return p1->check(val) || p2->check(val);
}

bool joda::query::OrPredicate::isCompatible(Predicate* other) {
  auto* pred = dynamic_cast<joda::query::OrPredicate*>(other);
  return pred != nullptr && ((this->p1->isCompatible(pred->p1.get()) &&
                              this->p2->isCompatible(pred->p2.get())) ||
                             (this->p1->isCompatible(pred->p2.get()) &&
                              this->p2->isCompatible(pred->p1.get())));
}

const std::string joda::query::OrPredicate::type = "OrPredicate";

std::string joda::query::OrPredicate::getType() { return OrPredicate::type; }

void joda::query::OrPredicate::accept(class joda::query::PredicateVisitor& v) {
  v.visit(this);
}

void joda::query::OrPredicate::subAccept(class joda::query::PredicateVisitor& v,
                                         bool lhs) {
  if (lhs) {
    p1->accept(v);
  } else {
    p2->accept(v);
  }
}
