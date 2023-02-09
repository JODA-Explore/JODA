#pragma once

#include <joda/indexing/AdaptiveIndexQuery.h>
#include <joda/container/JSONContainer.h>
#include <joda/query/values/IValueProvider.h>
#include <joda/query/values/TruthyFalsyProvider.h>
#include <joda/query/values/EqualityProvider.h>
#include <joda/query/values/ComparisonProvider.h>
#include <joda/query/values/BinaryBoolProvider.h>
#include <joda/query/values/NotProvider.h>
#include "../../../src/adaptive/visitors/AdaptiveQueryVisitor.h"

class AdaptiveIndexQueryNode {
public:
    virtual ~AdaptiveIndexQueryNode() = default;
    virtual void accept(class AdaptiveQueryVisitor &v) = 0;

    bool negated = false;
};

class BinaryAdaptiveIndexQueryNode : public AdaptiveIndexQueryNode {
public:
    virtual ~BinaryAdaptiveIndexQueryNode() = default;
    virtual void subAccept(AdaptiveQueryVisitor &v, bool isLhs) = 0;
    std::shared_ptr<AdaptiveIndexQueryNode> lhs;
    std::shared_ptr<AdaptiveIndexQueryNode> rhs;
};

class AndAdaptiveIndexQueryNode : public BinaryAdaptiveIndexQueryNode {
public:
    virtual ~AndAdaptiveIndexQueryNode() = default;
    void accept(class AdaptiveQueryVisitor &v) override;

    void subAccept(AdaptiveQueryVisitor &v, bool isLhs) override {
        if (isLhs) {
            lhs->accept(v);
        } else {
            rhs->accept(v);
        }
    }
};

class OrAdaptiveIndexQueryNode : public BinaryAdaptiveIndexQueryNode {
public:
    virtual ~OrAdaptiveIndexQueryNode() = default;
    void accept(class AdaptiveQueryVisitor &v) override;

    void subAccept(AdaptiveQueryVisitor &v, bool isLhs) override {
        if (isLhs) {
            lhs->accept(v);
        } else {
            rhs->accept(v);
        }
    }
};

class ExecutableAdaptiveIndexQueryNode : public AdaptiveIndexQueryNode {
public:
    virtual ~ExecutableAdaptiveIndexQueryNode() = default;
    void accept(class AdaptiveQueryVisitor &v);

    std::unique_ptr<AdaptiveIndexQuery> query;
    std::unique_ptr<joda::query::IValueProvider> predicate;
};

class UnsupportedAdaptiveIndexQueryNode : public AdaptiveIndexQueryNode {
public:
    virtual ~UnsupportedAdaptiveIndexQueryNode() = default;
    void accept(class AdaptiveQueryVisitor &v);
    std::unique_ptr<joda::query::IValueProvider> predicate;
};

class AdaptiveIndexQueryTreeVisitor {
public:
    void visit(joda::query::IValueProvider *e) ;
    void visit(joda::query::AndProvider *e) ;
    void visit(joda::query::NotProvider *e) ;
    void visit(joda::query::OrProvider *e) ;
    void visit(joda::query::LessProvider *e) ;
    void visit(joda::query::GreaterProvider *e) ;
    void visit(joda::query::LessEqualProvider *e) ;
    void visit(joda::query::GreaterEqualProvider *e) ;
    void visit(joda::query::EqualProvider *e) ;
    void visit(joda::query::UnequalProvider *e) ;

    std::shared_ptr<AdaptiveIndexQueryNode> getRoot();
    bool hasNoSupportedIndex() { return !indexFound; };
    bool isIndexOnly() { return indexFound && !unsupportedFound; };
protected:
    std::shared_ptr<AdaptiveIndexQueryNode> queryRoot;
    std::shared_ptr<BinaryAdaptiveIndexQueryNode> workingNode;

    std::vector<ExecutableAdaptiveIndexQueryNode*> indexQueries;

    // Helper functions
    void addExecutableNode(std::unique_ptr<AdaptiveIndexQuery> query, std::unique_ptr<joda::query::IValueProvider> p);
    bool upFromConjunction = false;
    bool indexFound = false;
    bool unsupportedFound = false;
};