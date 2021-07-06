//
// Created by "Nico Schaefer" on 12/9/16.
//

#include "joda/query/predicate/NegatePredicate.h"
#include "../include/joda/query/predicate/AndPredicate.h"

bool joda::query::NegatePredicate::check(const RapidJsonDocument& val) {
  return !pred->check(val);
}

joda::query::NegatePredicate::NegatePredicate(std::unique_ptr<Predicate> pred)
    : pred(std::move(pred)) {}

void joda::query::NegatePredicate::setPredicate(
    std::unique_ptr<Predicate> pred) {
  this->pred = std::move(pred);
}

bool joda::query::NegatePredicate::isCompatible(Predicate* other) {
  auto* pred = dynamic_cast<NegatePredicate*>(other);
  return pred != nullptr && this->pred->isCompatible(pred);
}

std::string joda::query::NegatePredicate::getType() {
  return NegatePredicate::type;
}

const std::string joda::query::NegatePredicate::type = "NegatePredicate";

void joda::query::NegatePredicate::accept(
    class joda::query::PredicateVisitor& v) {
  v.visit(this);
}

void joda::query::NegatePredicate::subAccept(
    class joda::query::PredicateVisitor& v) {
  pred->accept(v);
}
