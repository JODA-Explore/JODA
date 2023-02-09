#pragma once

#include <joda/indexing/AdaptiveIndexQuery.h>
#include <joda/indexing/AdaptiveIndexQueryTreeVisitor.h>

class NextPredicateVisitor {
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

    std::unique_ptr<ExecutableAdaptiveIndexQueryNode> getNextPredicate();
protected:
    std::unique_ptr<ExecutableAdaptiveIndexQueryNode> nextPredicate;
};
