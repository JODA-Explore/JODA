#include "SetPredicateVisitor.h"

#include "../util/AdaptiveQueryCreator.h"

void SetPredicateVisitor::visit(joda::query::IValueProvider *e) {
  auto name = e->getName();
  if (name == "LESS") {
    return visit(static_cast<joda::query::LessProvider *>(e));
  } else if (name == "LESSEQ") {
    return visit(static_cast<joda::query::LessEqualProvider *>(e));
  } else if (name == "GREATEREQ") {
    return visit(static_cast<joda::query::GreaterEqualProvider *>(e));
  } else if (name == "GREATER") {
    return visit(static_cast<joda::query::GreaterProvider *>(e));
  } else if (name == "EQUAL") {
    return visit(static_cast<joda::query::EqualProvider *>(e));
  } else if (name == "UNEQUAL") {
    return visit(static_cast<joda::query::UnequalProvider *>(e));
  } else if (name == "AND") {
    return visit(static_cast<joda::query::AndProvider *>(e));
  } else if (name == "OR") {
    return visit(static_cast<joda::query::OrProvider *>(e));
  } else if (name == "NOT") {
    return visit(static_cast<joda::query::NotProvider *>(e));
  } else {
    // If first index query found -> replace with const value
    if (!changed && AdaptiveQueryCreator::createQuery(e)) {
      changed = true;
      pred = std::make_unique<joda::query::BoolProvider>(value);
      return;
    }

    DCHECK(pred == nullptr);
    pred = e->duplicate();
    DCHECK(pred != nullptr);
  }
}

void SetPredicateVisitor::visit(joda::query::AndProvider *e) {
  std::vector<std::unique_ptr<joda::query::IValueProvider>> params;

  DCHECK(pred == nullptr);
  auto &lhs = e->getLhs();
  visit(lhs.get());
  DCHECK(pred != nullptr);
  params.emplace_back(std::move(pred));
  DCHECK(pred == nullptr);

  DCHECK(pred == nullptr);
  auto &rhs = e->getRhs();
  visit(rhs.get());
  DCHECK(pred != nullptr);
  params.emplace_back(std::move(pred));
  DCHECK(pred == nullptr);

  DCHECK(lhs != nullptr && rhs != nullptr);

  pred = std::make_unique<joda::query::AndProvider>(std::move(params));
  DCHECK(pred != nullptr);
}

void SetPredicateVisitor::visit(joda::query::NotProvider *e) {
  std::vector<std::unique_ptr<joda::query::IValueProvider>> params;

  DCHECK(pred == nullptr);
  auto &sub = e->getSub();
  visit(sub.get());
  DCHECK(pred != nullptr);
  params.emplace_back(std::move(pred));
  DCHECK(pred == nullptr);

  pred = std::make_unique<joda::query::NotProvider>(std::move(params));
  DCHECK(pred != nullptr);
}

void SetPredicateVisitor::visit(joda::query::OrProvider *e) {
  std::vector<std::unique_ptr<joda::query::IValueProvider>> params;

  DCHECK(pred == nullptr);
  auto &lhs = e->getLhs();
  visit(lhs.get());
  DCHECK(pred != nullptr);
  params.emplace_back(std::move(pred));
  DCHECK(pred == nullptr);

  DCHECK(pred == nullptr);
  auto &rhs = e->getRhs();
  visit(rhs.get());
  DCHECK(pred != nullptr);
  params.emplace_back(std::move(pred));
  DCHECK(pred == nullptr);

  DCHECK(lhs != nullptr && rhs != nullptr);

  pred = std::make_unique<joda::query::OrProvider>(std::move(params));
  DCHECK(pred != nullptr);
}

void SetPredicateVisitor::visit(joda::query::LessProvider *e) {
  // If first index query found -> replace with const value
  if (!changed && AdaptiveQueryCreator::createQuery(e)) {
    changed = true;
    pred = std::make_unique<joda::query::BoolProvider>(value);
    return;
  }

  pred = e->duplicate();
  DCHECK(pred != nullptr);
}

void SetPredicateVisitor::visit(joda::query::GreaterProvider *e) {
  // If first index query found -> replace with const value
  if (!changed && AdaptiveQueryCreator::createQuery(e)) {
    changed = true;
    pred = std::make_unique<joda::query::BoolProvider>(value);
    return;
  }

  pred = e->duplicate();
  DCHECK(pred != nullptr);
}

void SetPredicateVisitor::visit(joda::query::LessEqualProvider *e) {
  // If first index query found -> replace with const value
  if (!changed && AdaptiveQueryCreator::createQuery(e)) {
    changed = true;
    pred = std::make_unique<joda::query::BoolProvider>(value);
    return;
  }

  pred = e->duplicate();
  DCHECK(pred != nullptr);
}

void SetPredicateVisitor::visit(joda::query::GreaterEqualProvider *e) {
  // If first index query found -> replace with const value
  if (!changed && AdaptiveQueryCreator::createQuery(e)) {
    changed = true;
    pred = std::make_unique<joda::query::BoolProvider>(value);
    return;
  }

  pred = e->duplicate();
  DCHECK(pred != nullptr);
}

void SetPredicateVisitor::visit(joda::query::EqualProvider *e) {
  DCHECK(pred == nullptr);

  // If first index query found -> replace with const value
  if (!changed && AdaptiveQueryCreator::createQuery(e)) {
    changed = true;
    pred = std::make_unique<joda::query::BoolProvider>(value);
    return;
  }

  pred = e->duplicate();
  DCHECK(pred != nullptr);
}

void SetPredicateVisitor::visit(joda::query::UnequalProvider *e) {
  DCHECK(pred == nullptr);

  // If first index query found -> replace with const value
  if (!changed && AdaptiveQueryCreator::createQuery(e)) {
    changed = true;
    pred = std::make_unique<joda::query::BoolProvider>(value);
    return;
  }

  pred = e->duplicate();
  DCHECK(pred != nullptr);
}

std::unique_ptr<joda::query::IValueProvider> SetPredicateVisitor::getPred() {
  DCHECK(pred != nullptr);
  return std::move(pred);
}

SetPredicateVisitor::SetPredicateVisitor(bool value) : value(value) {}
