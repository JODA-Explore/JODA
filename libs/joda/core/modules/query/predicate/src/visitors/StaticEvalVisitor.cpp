//
// Created by Nico Schäfer on 12/13/17.
//

#include <glog/logging.h>
#include <joda/query/values/AtomProvider.h>
#include "../../include/joda/query/predicate/StaticEvalVisitor.h"
#include "../../include/joda/query/predicate/AndPredicate.h"
#include "joda/query/predicate/NegatePredicate.h"
#include "../../include/joda/query/predicate/OrPredicate.h"
#include "../../include/joda/query/predicate/EqualizePredicate.h"
#include "../../include/joda/query/predicate/ComparePredicate.h"
#include "../../include/joda/query/predicate/ValToPredicate.h"

void joda::query::StaticEvalVisitor::visit(joda::query::AndPredicate *e) {
  DCHECK(pred == nullptr);
  e->subAccept(*this, true);
  DCHECK(pred != nullptr);
  auto lhs = std::move(pred);
  DCHECK(pred == nullptr);

  DCHECK(pred == nullptr);
  e->subAccept(*this, false);
  DCHECK(pred != nullptr);
  auto rhs = std::move(pred);
  DCHECK(pred == nullptr);

  DCHECK(lhs != nullptr && rhs != nullptr);

  //Check LHS for const Bool
  auto *lhsVal = dynamic_cast<joda::query::ValToPredicate *> (lhs.get());
  bool lhsTrue = false;
  bool lhsFalse = false;
  bool lhsConst = false;
  if (lhsVal != nullptr) {
    lhsTrue = lhsVal->isConstTrue();
    lhsFalse = lhsVal->isConstFalse();
    lhsConst = lhsTrue || lhsFalse;
  }

  //Check RHS for const Bool
  auto *rhsVal = dynamic_cast<joda::query::ValToPredicate *> (rhs.get());
  bool rhsTrue = false;
  bool rhsFalse = false;
  bool rhsConst = false;
  if (rhsVal != nullptr) {
    rhsTrue = rhsVal->isConstTrue();
    rhsFalse = rhsVal->isConstFalse();
    rhsConst = rhsTrue || rhsFalse;
  }

  if (lhsConst && rhsConst) { //If both Const: X && Y => True/False
    pred = std::make_unique<joda::query::ValToPredicate>(std::make_unique<BoolProvider>(lhsTrue && rhsTrue));
    LOG(INFO) << std::string(lhsTrue ? JODA_TRUE_STRING : JODA_FALSE_STRING) << " && "
              << (lhsTrue ? JODA_TRUE_STRING : JODA_FALSE_STRING) << " => "
              << (lhsTrue && rhsTrue ? JODA_TRUE_STRING : JODA_FALSE_STRING);
    changed = true;
  } else if (lhsConst) { //If only LHS Const
    if (lhsFalse) { //If LHS = False => X && Y = False
      pred = std::make_unique<joda::query::ValToPredicate>(false);
      LOG(INFO) << "False && " << rhs->getType() << " => False";
      changed = true;
    } else { //If X = true => X && Y => Y
      LOG(INFO)<<"True && " << rhs->getType() << " => " << rhs->getType();
      pred = std::move(rhs);
      changed = true;
    }
  } else if (rhsConst) {
    if (rhsFalse) { //If RHS = False => X && Y = False
      pred = std::make_unique<joda::query::ValToPredicate>(false);
      LOG(INFO) << lhs->getType() << " && False => False";
      changed = true;
    } else { //If Y = true => X && Y => X
      LOG(INFO) << lhs->getType() << " && True => " << lhs->getType();
      pred = std::move(lhs);
      changed = true;
    }
  } else {
    pred = std::make_unique<joda::query::AndPredicate>(std::move(lhs), std::move(rhs));
  }
  DCHECK(pred != nullptr);
}
void joda::query::StaticEvalVisitor::visit(joda::query::NegatePredicate *e) {
  DCHECK(pred == nullptr);
  e->subAccept(*this);
  DCHECK(pred != nullptr);
  auto tmp = std::move(pred);
  DCHECK(pred == nullptr);
  auto *tmpVal = dynamic_cast<joda::query::ValToPredicate *> (tmp.get());
  if (tmpVal != nullptr) {
    bool tmpTrue = tmpVal->isConstTrue();
    bool tmpFalse = tmpVal->isConstFalse();
    bool tmpConst = tmpTrue || tmpFalse;
    if (tmpConst) {
      { //If subPred == true => False
        pred = std::make_unique<joda::query::ValToPredicate>(!tmpTrue);
        LOG(INFO) << std::string("!") << (tmpTrue ? JODA_TRUE_STRING : JODA_FALSE_STRING) << " => "
                  << (!tmpTrue ? JODA_TRUE_STRING : JODA_FALSE_STRING);
        changed = true;
        DCHECK(pred != nullptr);
        return;
      }
    }
  }

  pred = std::make_unique<joda::query::NegatePredicate>(std::move(tmp));
  DCHECK(pred != nullptr);
}
void joda::query::StaticEvalVisitor::visit(joda::query::OrPredicate *e) {
  DCHECK(pred == nullptr);
  e->subAccept(*this, true);
  DCHECK(pred != nullptr);
  auto lhs = std::move(pred);
  DCHECK(pred == nullptr);

  e->subAccept(*this, false);
  DCHECK(pred != nullptr);
  auto rhs = std::move(pred);
  DCHECK(pred == nullptr);

  DCHECK(lhs != nullptr && rhs != nullptr);

  //Check LHS for const Bool
  auto *lhsVal = dynamic_cast<joda::query::ValToPredicate *> (lhs.get());
  bool lhsTrue = false;
  bool lhsFalse = false;
  bool lhsConst = false;
  if (lhsVal != nullptr) {
    lhsTrue = lhsVal->isConstTrue();
    lhsFalse = lhsVal->isConstFalse();
    lhsConst = lhsTrue || lhsFalse;
  }

  //Check RHS for const Bool
  auto *rhsVal = dynamic_cast<joda::query::ValToPredicate *> (rhs.get());
  bool rhsTrue = false;
  bool rhsFalse = false;
  bool rhsConst = false;
  if (rhsVal != nullptr) {
    rhsTrue = rhsVal->isConstTrue();
    rhsFalse = rhsVal->isConstFalse();
    rhsConst = rhsTrue || rhsFalse;
  }

  if (lhsConst && rhsConst) { //If both Const: X || Y => True/False
    pred = std::make_unique<joda::query::ValToPredicate>(std::make_unique<BoolProvider>(lhsTrue || rhsTrue));
    LOG(INFO) << std::string(lhsTrue ? JODA_TRUE_STRING : JODA_FALSE_STRING) << " || "
              << (lhsTrue ? JODA_TRUE_STRING : JODA_FALSE_STRING) << " => "
              << (lhsTrue || rhsTrue ? JODA_TRUE_STRING : JODA_FALSE_STRING);
    changed = true;
  } else if (lhsConst) { //If only LHS Const
    if (lhsTrue) { //If LHS = True => X || Y = True
      pred = std::make_unique<joda::query::ValToPredicate>(true);
      LOG(INFO) << "True || " << rhs->getType() << " => True";
      changed = true;
    } else { //If X = true => X || Y => Y
      LOG(INFO) << "False || " << rhs->getType() << " => " << rhs->getType();
      pred = std::move(rhs);
      changed = true;
    }
  } else if (rhsConst) {
    if (rhsTrue) { //If RHS = True => X || Y = True
      pred = std::make_unique<joda::query::ValToPredicate>(true);
      LOG(INFO) << lhs->getType() << " || True => True";
      changed = true;
    } else { //If Y = true => X || Y => X
      LOG(INFO) << lhs->getType() << " || False => " << lhs->getType();
      pred = std::move(lhs);
      changed = true;
    }
  } else {
    pred = std::make_unique<joda::query::OrPredicate>(std::move(lhs), std::move(rhs));
  }
  DCHECK(pred != nullptr);
}

void joda::query::StaticEvalVisitor::visit(joda::query::EqualizePredicate *e) {
  DCHECK(pred == nullptr);
  auto &lhs = e->getLhs();
  auto &rhs = e->getRhs();

  if (lhs->isConst() && rhs->isConst()) {
    bool eq = lhs->equal(rhs.get(), RapidJsonDocument());
    pred = std::make_unique<joda::query::ValToPredicate>(eq == e->isEqual());
    LOG(INFO) << lhs->toString() << (e->isEqual() ? "==" : "!=") << rhs->toString() << " => "
              << (eq == e->isEqual() ? JODA_TRUE_STRING : JODA_FALSE_STRING);
    changed = true;
  } else {
    pred = std::make_unique<joda::query::EqualizePredicate>(std::move(lhs), std::move(rhs), e->isEqual());
  }
  DCHECK(pred != nullptr);
}
void joda::query::StaticEvalVisitor::visit(joda::query::ComparePredicate *e) {
  DCHECK(pred == nullptr);
  auto &lhs = e->getLhs();
  auto &rhs = e->getRhs();
  if (lhs->isConst() && rhs->isConst()) {
    pred = std::make_unique<joda::query::ValToPredicate>(e->check(RapidJsonDocument()));
    LOG(INFO) << lhs->toString() << (e->isGreater() ? ">" : "<") << (e->isInclude() ? "=" : "") << rhs->toString() << " => "
              << (e->check(RapidJsonDocument()) ? JODA_TRUE_STRING : JODA_FALSE_STRING);
    changed = true;
  } else {
    pred = std::make_unique<joda::query::ComparePredicate>(std::move(lhs), std::move(rhs), e->isGreater(), e->isInclude());
  }
  DCHECK(pred != nullptr);
}
void joda::query::StaticEvalVisitor::visit(joda::query::ValToPredicate *e) {
  DCHECK(pred == nullptr);
  auto val = e->duplicateIVal();
  IValueProvider::replaceConstSubexpressions(val);
  pred = std::make_unique<joda::query::ValToPredicate>(std::move(val));
  DCHECK(pred != nullptr);
}
std::unique_ptr<joda::query::Predicate> joda::query::StaticEvalVisitor::getPred() {
  DCHECK(pred != nullptr);
  return std::move(pred);
}
