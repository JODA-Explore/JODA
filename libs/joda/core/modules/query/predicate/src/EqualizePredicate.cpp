//
// Created by Nico Schäfer on 11/13/17.
//

#include "../include/joda/query/predicate/EqualizePredicate.h"
#include "joda/query/values/PointerProvider.h"

bool joda::query::EqualizePredicate::check(const RapidJsonDocument &val) {

  return lhs->equal(rhs.get(), val) == equal;
}

bool joda::query::EqualizePredicate::isCompatible(joda::query::Predicate *other) {
  return Predicate::isCompatible(other);
}

std::string joda::query::EqualizePredicate::getType() {
  return "EqualPredicate";
}

void joda::query::EqualizePredicate::accept(class joda::query::PredicateVisitor &v) {
  v.visit(this);
}

joda::query::EqualizePredicate::EqualizePredicate(std::unique_ptr<IValueProvider> &&lhs,
                                                  std::unique_ptr<IValueProvider> &&rhs,
                                                  bool equal) :
    lhs(std::move(lhs)), rhs(std::move(rhs)), equal(equal) {
  if (!this->lhs->equalizable() || !this->rhs->equalizable()) throw joda::query::NotEqualizableException();

}
std::string joda::query::EqualizePredicate::toString() {
  std::string comp;
  if (equal) comp = "==";
  else comp = "!=";
  return lhs->toString() +" "+ comp +" "+ rhs->toString();
}
std::vector<std::string> joda::query::EqualizePredicate::getAttributes() const {
  std::vector<std::string> ret;
  auto *tmp = dynamic_cast<PointerProvider *>(lhs.get());
  if (tmp != nullptr) ret.push_back(tmp->getAttributeString());

  tmp = dynamic_cast<PointerProvider *>(rhs.get());
  if (tmp != nullptr) ret.push_back(tmp->getAttributeString());
  return ret;
}
std::unique_ptr<joda::query::Predicate> joda::query::EqualizePredicate::duplicate() {
  return std::make_unique<joda::query::EqualizePredicate>(lhs->duplicate(), rhs->duplicate(), equal);
}
std::unique_ptr<joda::query::IValueProvider> &joda::query::EqualizePredicate::getLhs() {
  return lhs;
}
std::unique_ptr<joda::query::IValueProvider> &joda::query::EqualizePredicate::getRhs() {
  return rhs;
}
bool joda::query::EqualizePredicate::isEqual() const {
  return equal;
}
