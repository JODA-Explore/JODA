// pred
// Created by "Nico Schaefer" on 11/25/16.
//

#include "../include/joda/query/predicate/Predicate.h"

bool joda::query::Predicate::isCompatible(joda::query::Predicate* other) {
  return this->getType() == other->getType();
}

std::string joda::query::Predicate::getType() { return Predicate::type; }

const std::string joda::query::Predicate::type = "Predicate";
