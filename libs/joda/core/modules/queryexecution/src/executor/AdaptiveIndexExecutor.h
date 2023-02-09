#pragma once

#include <joda/indexing/AdaptiveIndexQueryTreeVisitor.h>
#include <joda/queryexecution/IQueryExecutor.h>
#include "joda/query/Query.h"
#include "joda/config/config.h"
#include "joda/container/JSONContainer.h"
#include <joda/indexing/AdaptiveIndexManager.h>

class AdaptiveIndexExecutor : public IQueryExecutor {
public:
	explicit AdaptiveIndexExecutor(const joda::query::Query& q);
	unsigned long estimatedWork(const joda::query::Query& q, JSONContainer& cont) override;
	std::shared_ptr<const DocIndex> execute(const joda::query::Query& q, JSONContainer& cont) override;
	std::string getName() const override;
	void alwaysAfterSelect(const joda::query::Query& q, std::shared_ptr<const DocIndex>& sel, JSONContainer& cont) override;
	std::unique_ptr<IQueryExecutor> duplicate() override;

protected:
    AdaptiveIndexExecutor(std::shared_ptr<AdaptiveIndexQueryTreeVisitor>& queryTreeVisitor);

    std::shared_ptr<const DocIndex> executeNextMixedQuery(joda::query::Query &q, JSONContainer& cont, std::shared_ptr<const DocIndex> docIndex);

    std::shared_ptr<AdaptiveIndexQueryTreeVisitor> queryTreeVisitor;
    std::unique_ptr<DataContext> dataContext;

private:
    size_t estimateIndexEffort(const std::shared_ptr<AdaptiveIndexQueryNode>& root, JSONContainer& cont);
};