#pragma once

#include "../../../src/adaptive/visitors/AdaptiveQueryVisitor.h"
#include "../../../src/adaptive/environment/DataContext.h"

class AdaptiveQueryExecutorVisitor : public AdaptiveQueryVisitor {
public:

    AdaptiveQueryExecutorVisitor(JSONContainer& container);

    void visit(AndAdaptiveIndexQueryNode *node) override;
    void visit(OrAdaptiveIndexQueryNode *node) override;
    void visit(ExecutableAdaptiveIndexQueryNode *node) override;
    void visit(UnsupportedAdaptiveIndexQueryNode *node) override;

    std::shared_ptr<const DocIndex> getResult();

private:
    JSONContainer& container;
    std::unique_ptr<DataContext> dataContext;

    std::shared_ptr<const DocIndex> result;
};