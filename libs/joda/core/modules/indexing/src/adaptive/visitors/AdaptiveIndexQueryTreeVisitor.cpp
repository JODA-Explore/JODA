#include "joda/indexing/AdaptiveIndexQueryTreeVisitor.h"

#include <iostream>

#include "../util/AdaptiveQueryCreator.h"

void AdaptiveIndexQueryTreeVisitor::visit(joda::query::AndProvider *e) {
  std::shared_ptr<BinaryAdaptiveIndexQueryNode> bNode =
      std::make_shared<AndAdaptiveIndexQueryNode>();

  if (!queryRoot) {
    queryRoot = bNode;
  }

  if (workingNode) {
    if (workingNode->lhs) {
      workingNode->rhs = bNode;
    } else {
      workingNode->lhs = bNode;
    }
  }

  workingNode = bNode;

  auto &lhs = e->getLhs();
  visit(lhs.get());

  workingNode = bNode;

  auto &rhs = e->getRhs();
  visit(rhs.get());

  workingNode = bNode;
  upFromConjunction = true;
}

void AdaptiveIndexQueryTreeVisitor::visit(joda::query::NotProvider *e) {
  auto &predicate = e->getSub();
  visit(predicate.get());

  // last node added is negative
  if (!workingNode) {  // only one root with negate
    queryRoot->negated = true;
  } else if (upFromConjunction) {
    workingNode->negated = true;
  } else {
    if (workingNode->lhs) {
      if (workingNode->rhs) {
        // rhs set -> last node is rhs
        workingNode->rhs->negated = true;
      } else {
        // only lhs set -> last node is lhs
        workingNode->lhs->negated = true;
      }
    } else {
      // whole and/or negated
      workingNode->negated = true;
    }
  }
}

void AdaptiveIndexQueryTreeVisitor::visit(joda::query::OrProvider *e) {
  std::shared_ptr<BinaryAdaptiveIndexQueryNode> bNode =
      std::make_shared<OrAdaptiveIndexQueryNode>();

  if (!queryRoot) {
    queryRoot = bNode;
  }

  if (workingNode) {
    if (workingNode->lhs) {
      workingNode->rhs = bNode;
    } else {
      workingNode->lhs = bNode;
    }
  }

  workingNode = bNode;

  auto &lhs = e->getLhs();
  visit(lhs.get());

  workingNode = bNode;

  auto &rhs = e->getRhs();
  visit(rhs.get());

  workingNode = bNode;
  upFromConjunction = true;
}

void AdaptiveIndexQueryTreeVisitor::visit(joda::query::LessProvider *e) {
  std::unique_ptr<AdaptiveIndexQuery> query =
      AdaptiveQueryCreator::createQuery(e);
  addExecutableNode(std::move(query), e->duplicate());
  upFromConjunction = false;
}

void AdaptiveIndexQueryTreeVisitor::visit(joda::query::GreaterProvider *e) {
  std::unique_ptr<AdaptiveIndexQuery> query =
      AdaptiveQueryCreator::createQuery(e);
  addExecutableNode(std::move(query), e->duplicate());
  upFromConjunction = false;
}

void AdaptiveIndexQueryTreeVisitor::visit(joda::query::LessEqualProvider *e) {
  std::unique_ptr<AdaptiveIndexQuery> query =
      AdaptiveQueryCreator::createQuery(e);
  addExecutableNode(std::move(query), e->duplicate());
  upFromConjunction = false;
}

void AdaptiveIndexQueryTreeVisitor::visit(
    joda::query::GreaterEqualProvider *e) {
  std::unique_ptr<AdaptiveIndexQuery> query =
      AdaptiveQueryCreator::createQuery(e);
  addExecutableNode(std::move(query), e->duplicate());
  upFromConjunction = false;
}

void AdaptiveIndexQueryTreeVisitor::visit(joda::query::EqualProvider *e) {
  std::unique_ptr<AdaptiveIndexQuery> query =
      AdaptiveQueryCreator::createQuery(e);
  addExecutableNode(std::move(query), e->duplicate());
  upFromConjunction = false;
}

void AdaptiveIndexQueryTreeVisitor::visit(joda::query::UnequalProvider *e) {
  std::unique_ptr<AdaptiveIndexQuery> query =
      AdaptiveQueryCreator::createQuery(e);
  addExecutableNode(std::move(query), e->duplicate());
  upFromConjunction = false;
}

void AdaptiveIndexQueryTreeVisitor::visit(joda::query::IValueProvider *e) {
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
    std::unique_ptr<AdaptiveIndexQuery> query =
        AdaptiveQueryCreator::createQuery(e);
    addExecutableNode(std::move(query), e->duplicate());
    upFromConjunction = false;
  }
}

void AdaptiveIndexQueryTreeVisitor::addExecutableNode(
    std::unique_ptr<AdaptiveIndexQuery> query,
    std::unique_ptr<joda::query::IValueProvider> p) {
  std::unique_ptr<AdaptiveIndexQueryNode> node;

  if (query) {
    std::unique_ptr<ExecutableAdaptiveIndexQueryNode> enode =
        std::make_unique<ExecutableAdaptiveIndexQueryNode>();
    indexQueries.push_back(enode.get());
    enode->query = std::move(query);
    enode->predicate = std::move(p);
    node = std::move(enode);
    indexFound = true;
  } else {
    std::unique_ptr<UnsupportedAdaptiveIndexQueryNode> unode =
        std::make_unique<UnsupportedAdaptiveIndexQueryNode>();
    unode->predicate = std::move(p);
    node = std::move(unode);
    unsupportedFound = true;
  }

  if (workingNode) {
    if (workingNode->lhs) {
      workingNode->rhs = std::move(node);
    } else {
      workingNode->lhs = std::move(node);
    }
  } else {
    queryRoot = std::move(node);
  }
}

std::shared_ptr<AdaptiveIndexQueryNode>
AdaptiveIndexQueryTreeVisitor::getRoot() {
  return queryRoot;
}

void UnsupportedAdaptiveIndexQueryNode::accept(class AdaptiveQueryVisitor &v) {
  v.visit(this);
}

void AndAdaptiveIndexQueryNode::accept(class AdaptiveQueryVisitor &v) {
  v.visit(this);
}

void OrAdaptiveIndexQueryNode::accept(class AdaptiveQueryVisitor &v) {
  v.visit(this);
}

void ExecutableAdaptiveIndexQueryNode::accept(class AdaptiveQueryVisitor &v) {
  v.visit(this);
}
