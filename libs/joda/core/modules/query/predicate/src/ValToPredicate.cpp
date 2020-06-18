//
// Created by Nico Schäfer on 11/23/17.
//

#include <joda/query/values/AtomProvider.h>
#include "../include/joda/query/predicate/ValToPredicate.h"

bool joda::query::ValToPredicate::check(const RapidJsonDocument &val) {

  if (this->val->isBool()) {
    RJMemoryPoolAlloc alloc;
    auto tmp = this->val->getAtomValue(val, alloc);
    return tmp.IsBool() && tmp.GetBool();
  } else if (this->val->isAny()) {
    RJMemoryPoolAlloc alloc;
    auto tmp = this->val->getValue(val, alloc);
    if (tmp == nullptr) return false;
    if (!tmp->IsBool()) return false;
    return tmp->GetBool();
  }

  return false;
}
bool joda::query::ValToPredicate::isCompatible(Predicate *other) {
  return Predicate::isCompatible(other);
}
std::string joda::query::ValToPredicate::getType() {
  return ValToPredicate::type;
}
void joda::query::ValToPredicate::accept(class joda::query::PredicateVisitor &v) {
  v.visit(this);
}
const std::string joda::query::ValToPredicate::type = "ValToPredicate";
joda::query::ValToPredicate::ValToPredicate(std::unique_ptr<IValueProvider> &&val) : val(std::move(val)) {
  assert(this->val != nullptr);
  if (!(this->val->isAny() || this->val->isBool()))
    throw WrongParameterException("Expression does not evaluate to Boolean");
  IValueProvider::constBoolCheck(this->val);
}
std::unique_ptr<joda::query::ValToPredicate> joda::query::ValToPredicate::duplicate() const {
  return std::make_unique<joda::query::ValToPredicate>(std::move(val->duplicate()));
}
std::string joda::query::ValToPredicate::toString() const {
  return val->toString();
}
joda::query::ValToPredicate::ValToPredicate(bool b) {
  val = std::make_unique<BoolProvider>(b);
}
bool joda::query::ValToPredicate::isConstTrue() const {
  return val->toString() == "true";
}
bool joda::query::ValToPredicate::isConstFalse() const {
  return val->toString() == "false";
}

void joda::query::ValToPredicate::getAttributes(std::vector<std::string> &vec) const {
  val->getAttributes(vec);
}

std::unique_ptr<joda::query::IValueProvider> joda::query::ValToPredicate::duplicateIVal() const {
  return val->duplicate();
}
