//
// Created by Nico Schäfer on 11/13/17.
//

#include "../include/joda/query/predicate/ComparePredicate.h"
#include "joda/query/values/PointerProvider.h"

joda::query::ComparePredicate::ComparePredicate(std::unique_ptr<IValueProvider> &&lhs,
                                                std::unique_ptr<IValueProvider> &&rhs,
                                                bool greater,
                                                bool include) :
    lhs(std::move(lhs)), rhs(std::move(rhs)), greater(greater), include(include) {
  if (!this->lhs->comparable() || !this->rhs->comparable()) throw NotComparableException();
}
bool joda::query::ComparePredicate::check(const RapidJsonDocument &val) {
  //Check for String
  RJMemoryPoolAlloc tmpAlloc;
  const RJValue *lhslocal;
  RJValue tmplhslocal;
  const RJValue *rhslocal;
  RJValue tmprhslocal;
  //Get Pointer to value
  if (lhs->isAtom()) {
    tmplhslocal = lhs->getAtomValue(val, tmpAlloc);
    lhslocal = &tmplhslocal;
  } else {
    lhslocal = lhs->getValue(val, tmpAlloc);
  }
  if(lhslocal == nullptr) return false;
  //Get Pointer to List
  if (rhs->isAtom()) {
    tmprhslocal = rhs->getAtomValue(val, tmpAlloc);
    rhslocal = &tmprhslocal;
  } else {
    rhslocal = rhs->getValue(val, tmpAlloc);
  }
  if(rhslocal == nullptr) return false;

  if (lhslocal->IsString() && rhslocal->IsString()) {
    std::string lhsStr = lhslocal->GetString();
    std::string rhsStr = rhslocal->GetString();
    if (greater) {
      if (include) return lhsStr >= rhsStr;
      else return lhsStr > rhsStr;
    } else {
      if (include) return lhsStr <= rhsStr;
      else return lhsStr < rhsStr;
    }
  }

  if (lhslocal->IsNumber() && rhslocal->IsNumber()) {
    double lhsStr = lhslocal->GetDouble();
    double rhsStr = rhslocal->GetDouble();
    if (greater) {
      if (include) return lhsStr >= rhsStr;
      else return lhsStr > rhsStr;
    } else {
      if (include) return lhsStr <= rhsStr;
      else return lhsStr < rhsStr;
    }
  }

  return false;
}
bool joda::query::ComparePredicate::isCompatible(Predicate *other) {
  return Predicate::isCompatible(other);
}
std::string joda::query::ComparePredicate::getType() {
  return "ComparePredicate";
}
void joda::query::ComparePredicate::accept(class joda::query::PredicateVisitor &v) {
  v.visit(this);
}
std::string joda::query::ComparePredicate::toString() {
  std::string comp;
  if (greater) comp = ">";
  else comp = "<";
  if (include) comp += "=";
  return lhs->toString() +" "+ comp +" "+ rhs->toString();
}
std::vector<std::string> joda::query::ComparePredicate::getAttributes() const {
  std::vector<std::string> ret;
  auto *tmp = dynamic_cast<PointerProvider *>(lhs.get());
  if (tmp != nullptr) ret.push_back(tmp->getAttributeString());

  tmp = dynamic_cast<PointerProvider *>(rhs.get());
  if (tmp != nullptr) ret.push_back(tmp->getAttributeString());
  return ret;
}
std::unique_ptr<joda::query::Predicate> joda::query::ComparePredicate::duplicate() const {
  return std::make_unique<ComparePredicate>(lhs->duplicate(), rhs->duplicate(), greater, include);
}
bool joda::query::ComparePredicate::isGreater() const {
  return greater;
}
bool joda::query::ComparePredicate::isInclude() const {
  return include;
}

std::unique_ptr<joda::query::IValueProvider> &joda::query::ComparePredicate::getLhs() {
  return lhs;
}
std::unique_ptr<joda::query::IValueProvider> &joda::query::ComparePredicate::getRhs() {
  return rhs;
}
