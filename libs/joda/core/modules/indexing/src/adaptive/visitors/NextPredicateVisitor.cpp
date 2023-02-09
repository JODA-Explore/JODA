#include "NextPredicateVisitor.h"

#include "../util/AdaptiveQueryCreator.h"

void NextPredicateVisitor::visit(joda::query::IValueProvider *e) {
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
    auto query = AdaptiveQueryCreator::createQuery(e);

    if (query) {
      auto node = std::make_unique<ExecutableAdaptiveIndexQueryNode>();
      node->query = std::move(query);
      nextPredicate = std::move(node);
    }
  }
}

void NextPredicateVisitor::visit(joda::query::AndProvider *e) {
  auto &lhs = e->getLhs();
  visit(lhs.get());

  auto &rhs = e->getRhs();
  visit(rhs.get());
}

void NextPredicateVisitor::visit(joda::query::NotProvider *e) {
  auto &lhs = e->getSub();
  visit(lhs.get());
}

void NextPredicateVisitor::visit(joda::query::OrProvider *e) {
  auto &lhs = e->getLhs();
  visit(lhs.get());

  auto &rhs = e->getRhs();
  visit(rhs.get());
}

void NextPredicateVisitor::visit(joda::query::LessProvider *e) {
  auto query = AdaptiveQueryCreator::createQuery(e);

  if (query) {
    auto node = std::make_unique<ExecutableAdaptiveIndexQueryNode>();
    node->query = std::move(query);
    nextPredicate = std::move(node);
  }
}

void NextPredicateVisitor::visit(joda::query::GreaterProvider *e) {
  auto query = AdaptiveQueryCreator::createQuery(e);

  if (query) {
    auto node = std::make_unique<ExecutableAdaptiveIndexQueryNode>();
    node->query = std::move(query);
    nextPredicate = std::move(node);
  }
}

void NextPredicateVisitor::visit(joda::query::LessEqualProvider *e) {
  auto query = AdaptiveQueryCreator::createQuery(e);

  if (query) {
    auto node = std::make_unique<ExecutableAdaptiveIndexQueryNode>();
    node->query = std::move(query);
    nextPredicate = std::move(node);
  }
}

void NextPredicateVisitor::visit(joda::query::GreaterEqualProvider *e) {
  auto query = AdaptiveQueryCreator::createQuery(e);

  if (query) {
    auto node = std::make_unique<ExecutableAdaptiveIndexQueryNode>();
    node->query = std::move(query);
    nextPredicate = std::move(node);
  }
}

void NextPredicateVisitor::visit(joda::query::EqualProvider *e) {
  auto query = AdaptiveQueryCreator::createQuery(e);

  if (query) {
    auto node = std::make_unique<ExecutableAdaptiveIndexQueryNode>();
    node->query = std::move(query);
    nextPredicate = std::move(node);
  }
}

void NextPredicateVisitor::visit(joda::query::UnequalProvider *e) {
  auto query = AdaptiveQueryCreator::createQuery(e);

  if (query) {
    auto node = std::make_unique<ExecutableAdaptiveIndexQueryNode>();
    node->query = std::move(query);
    nextPredicate = std::move(node);
  }
}

std::unique_ptr<ExecutableAdaptiveIndexQueryNode>
NextPredicateVisitor::getNextPredicate() {
  return std::move(nextPredicate);
}
