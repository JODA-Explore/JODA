#include "AdaptiveIndexExecutor.h"

#include <joda/indexing/AdaptiveIndexQueryTreeVisitor.h>
#include <joda/indexing/AdaptiveQueryExecutorVisitor.h>

#include <iostream>

#include "../../../indexing/src/adaptive/visitors/NextPredicateVisitor.h"
#include "../../../indexing/src/adaptive/visitors/SetPredicateVisitor.h"

AdaptiveIndexExecutor::AdaptiveIndexExecutor(const joda::query::Query &q)
    : IQueryExecutor() {
  AdaptiveIndexQueryTreeVisitor visitor;
  visitor.visit(q.getChoose().get());
  queryTreeVisitor = std::make_shared<AdaptiveIndexQueryTreeVisitor>(visitor);
}

unsigned long AdaptiveIndexExecutor::estimatedWork(const joda::query::Query &q,
                                                   JSONContainer &cont) {
  if (!config::adaptiveIndex || queryTreeVisitor->hasNoSupportedIndex())
    return NOT_APPLICABLE;

  if (cont.getAdaptiveIndex() == nullptr) {
    cont.setAdaptiveIndex(std::make_unique<AdaptiveIndexManager>(cont.size()));
  }

  size_t estimatedWork = NOT_APPLICABLE;

  if (queryTreeVisitor->hasNoSupportedIndex()) {
    return estimatedWork;
  }

  size_t effort = estimateIndexEffort(queryTreeVisitor->getRoot(), cont);

  if (effort == NOT_APPLICABLE) {
    return estimatedWork;
  }

  // For now: Promote execution with index even if effort is higher
  if (effort > cont.size() - 1) {
    return cont.size() - 1;
  }

  return effort;
}

std::shared_ptr<const DocIndex> AdaptiveIndexExecutor::execute(
    const joda::query::Query &q, JSONContainer &cont) {
  if (queryTreeVisitor->isIndexOnly()) {
    AdaptiveQueryExecutorVisitor executorVisitor(cont);
    auto queryRoot = queryTreeVisitor->getRoot();
    queryRoot->accept(executorVisitor);
    return executorVisitor.getResult();
  } else if (queryTreeVisitor->hasNoSupportedIndex()) {
    // Default execution; should be prevented by estimate work function
    std::function<bool(RapidJsonDocument &, size_t)> fun =
        [&](RapidJsonDocument &doc, size_t i) { return q.check(doc); };
    return cont.checkDocuments(fun);
  } else {
    // Both types of predicates
    dataContext = std::make_unique<DataContext>(cont);
    std::shared_ptr<const DocIndex> allIds = cont.getAllIDs();

    joda::query::Query query;
    query.setChoose(q.getChoose());

    return executeNextMixedQuery(query, cont, allIds);
  }
}

std::string AdaptiveIndexExecutor::getName() const {
  return "AdaptiveIndexExecutor";
}

void AdaptiveIndexExecutor::alwaysAfterSelect(
    const joda::query::Query &q, std::shared_ptr<const DocIndex> &sel,
    JSONContainer &cont) {}

std::unique_ptr<IQueryExecutor> AdaptiveIndexExecutor::duplicate() {
  return std::unique_ptr<AdaptiveIndexExecutor>(
      new AdaptiveIndexExecutor(queryTreeVisitor));
}

AdaptiveIndexExecutor::AdaptiveIndexExecutor(
    std::shared_ptr<AdaptiveIndexQueryTreeVisitor> &treeVisitor)
    : queryTreeVisitor(treeVisitor) {}

std::shared_ptr<const DocIndex> AdaptiveIndexExecutor::executeNextMixedQuery(
    joda::query::Query &q, JSONContainer &cont,
    std::shared_ptr<const DocIndex> docIndex) {
  auto predicate = q.getChoose();

  // Is const bool
  bool constResult = false;
  if (joda::query::IValueProvider::constBoolCheck(predicate, constResult)) {
    if (constResult) {  // predicate is just constant True
      return docIndex;
    } else {
      return std::make_shared<const DocIndex>(cont.size());
    }
  }

  NextPredicateVisitor nextPredicateVisitor;
  nextPredicateVisitor.visit(predicate.get());

  auto nextAdaptiveQuery = nextPredicateVisitor.getNextPredicate();

  if (!nextAdaptiveQuery) {  // no next query
    std::function<bool(RapidJsonDocument &, size_t)> fun =
        [&](RapidJsonDocument &doc, size_t i) { return q.check(doc); };
    return dataContext->checkDocuments(fun, *docIndex);
  }

  std::shared_ptr<const DocIndex> indexResult =
      cont.getAdaptiveIndex()->executeQuery(*nextAdaptiveQuery->query,
                                            *dataContext);

  if (!indexResult) {
    std::cout << "query could not be executed. Falling back to default check. "
                 "Probably memory or query content failure."
              << std::endl;
    std::function<bool(RapidJsonDocument &, size_t)> fun =
        [&](RapidJsonDocument &doc, size_t i) { return q.check(doc); };
    return dataContext->checkDocuments(fun, *docIndex);
  }

  // filter the result to only the documents needed for this state
  DocIndex relevantDocs = *docIndex & *indexResult;
  auto result = std::make_shared<const DocIndex>(relevantDocs);

  // Copy predicates
  auto pT = q.getChoose();
  auto pF = q.getChoose();

  // Continue with true side
  SetPredicateVisitor setPredicateVisitor(true);
  setPredicateVisitor.visit(pT.get());
  q.setChoose(setPredicateVisitor.getPred());

  auto optimized = q.getChoose()->optimize();
  if (optimized != nullptr) {
    q.setChoose(std::move(optimized));
  }

  auto resultT = executeNextMixedQuery(q, cont, result);

  // Continue with false side
  setPredicateVisitor = SetPredicateVisitor(false);
  setPredicateVisitor.visit(pF.get());
  q.setChoose(setPredicateVisitor.getPred());

  optimized = q.getChoose()->optimize();
  if (optimized != nullptr) {
    q.setChoose(std::move(optimized));
  }

  // False bits: flip true bits + remove all documents with are not part of the
  // set (docIndex) to handle
  std::shared_ptr<DocIndex> indexPositives =
      std::make_shared<DocIndex>(relevantDocs);
  indexPositives->flip();  // all false hits from index
  std::shared_ptr<DocIndex> resultNeg =
      std::make_shared<DocIndex>(*indexPositives & *docIndex);

  auto resultF = executeNextMixedQuery(q, cont, resultNeg);

  return std::make_shared<const DocIndex>(*resultT | *resultF);
}

size_t AdaptiveIndexExecutor::estimateIndexEffort(
    const std::shared_ptr<AdaptiveIndexQueryNode> &queryNode,
    JSONContainer &cont) {
  auto binaryNode =
      dynamic_cast<BinaryAdaptiveIndexQueryNode *>(queryNode.get());

  if (binaryNode != nullptr) {
    auto effort1 = estimateIndexEffort(binaryNode->lhs, cont);
    auto effort2 = estimateIndexEffort(binaryNode->rhs, cont);

    if (effort1 == NOT_APPLICABLE || effort1 == NOT_APPLICABLE) {
      return NOT_APPLICABLE;
    }

    return effort1 + effort2;
  }

  auto executableNode =
      dynamic_cast<ExecutableAdaptiveIndexQueryNode *>(queryNode.get());

  if (executableNode != nullptr) {
    return cont.getAdaptiveIndex()->estimateWork(*executableNode->query);
  }

  return 0;
}
