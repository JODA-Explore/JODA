#pragma once

#include <joda/indexing/AdaptiveIndexQueryTreeVisitor.h>

class AndAdaptiveIndexQueryNode;
class OrAdaptiveIndexQueryNode;
class ExecutableAdaptiveIndexQueryNode;
class UnsupportedAdaptiveIndexQueryNode;

class AdaptiveQueryVisitor {
public:
    virtual void visit(AndAdaptiveIndexQueryNode *node) = 0;
    virtual void visit(OrAdaptiveIndexQueryNode *node) = 0;
    virtual void visit(ExecutableAdaptiveIndexQueryNode *node) = 0;
    virtual void visit(UnsupportedAdaptiveIndexQueryNode *node) = 0;
};