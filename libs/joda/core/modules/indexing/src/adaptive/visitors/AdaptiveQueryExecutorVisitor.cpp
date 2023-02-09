#include <iostream>
#include "joda/indexing/AdaptiveQueryExecutorVisitor.h"

void AdaptiveQueryExecutorVisitor::visit(AndAdaptiveIndexQueryNode *node) {
    node->subAccept(*this, true);

    auto lhsResult = std::move(result);

    node->subAccept(*this, false);

    auto rhsResult = std::move(result);

    DocIndex tmpResult = *lhsResult & *rhsResult;

    if (node->negated) {
        tmpResult.flip();
    }

   result = std::make_shared<const DocIndex>(tmpResult);
}

void AdaptiveQueryExecutorVisitor::visit(OrAdaptiveIndexQueryNode *node) {
    node->subAccept(*this, true);
    auto lhsResult = std::move(result);

    node->subAccept(*this, false);
    auto rhsResult = std::move(result);


    DocIndex tmpResult = *lhsResult | *rhsResult;

    if (node->negated) {
        tmpResult.flip();
    }

    result = std::make_shared<const DocIndex>(tmpResult);
}

void AdaptiveQueryExecutorVisitor::visit(ExecutableAdaptiveIndexQueryNode *node) {
    result = std::move(container.getAdaptiveIndex()->executeQuery(*node->query.get(), *dataContext.get()));

    if (!result) {
        std::cout << "query could not be executed. Falling back to default check. Probably memory or query content failure." << std::endl;

        auto *predicate = node->predicate.get();
        std::function<bool(RapidJsonDocument &, size_t)> fun = [predicate](RapidJsonDocument &doc, size_t i) {
            assert(predicate != nullptr);
            RJMemoryPoolAlloc alloc;
            return predicate->getNonTruthyBool(doc,alloc);
        };

        result = std::move(dataContext->checkDocuments(fun));
    }

    if (node->negated) {
        auto index = std::make_shared<DocIndex>(*result);
        index->flip();
        result = std::move(index);
    }
}

void AdaptiveQueryExecutorVisitor::visit(UnsupportedAdaptiveIndexQueryNode *node) {
    auto *predicate = node->predicate.get();

    std::function<bool(RapidJsonDocument &, size_t)> fun = [predicate](RapidJsonDocument &doc, size_t i) {
        assert(predicate != nullptr);
            RJMemoryPoolAlloc alloc;
            return predicate->getNonTruthyBool(doc,alloc);
    };

    result = std::move(dataContext->checkDocuments(fun));
}

AdaptiveQueryExecutorVisitor::AdaptiveQueryExecutorVisitor(JSONContainer &container) : container(container) {
    dataContext = std::make_unique<DataContext>(container);
}

std::shared_ptr<const DocIndex> AdaptiveQueryExecutorVisitor::getResult() {
    return result;
}
