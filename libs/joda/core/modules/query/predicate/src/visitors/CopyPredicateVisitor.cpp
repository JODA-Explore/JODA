//
// Created by Nico Schäfer on 6/26/17.
//

#include "../../include/joda/query/predicate/CopyPredicateVisitor.h"
#include "../../include/joda/query/predicate/AndPredicate.h"
#include "joda/query/predicate/NegatePredicate.h"
#include "../../include/joda/query/predicate/OrPredicate.h"
#include "../../include/joda/query/predicate/EqualizePredicate.h"
#include "../../include/joda/query/predicate/ComparePredicate.h"

#include "../../include/joda/query/predicate/ValToPredicate.h"


void joda::query::CopyPredicateVisitor::visit(joda::query::AndPredicate *e) {
    assert(pred == nullptr);
    e->p1->accept(*this);
    assert(pred != nullptr);
    auto lhs = std::move(pred);
    assert(pred == nullptr);
    e->p2->accept(*this);
    assert(pred != nullptr);
    auto rhs = std::move(pred);
    assert(pred == nullptr);
    pred = std::make_unique<joda::query::AndPredicate>(std::move(lhs), std::move(rhs));
    assert(pred != nullptr);
}

void joda::query::CopyPredicateVisitor::visit(NegatePredicate *e) {
        assert(pred == nullptr);
        e->pred->accept(*this);
        assert(pred != nullptr);
        auto tmp = std::make_unique<NegatePredicate>(std::move(pred));
        assert(pred == nullptr);
        pred = std::move(tmp);
        assert(pred != nullptr);
}

void joda::query::CopyPredicateVisitor::visit(OrPredicate *e) {
    assert(pred == nullptr);
    e->p1->accept(*this);
    assert(pred != nullptr);
    auto lhs = std::move(pred);
    assert(pred == nullptr);
    e->p2->accept(*this);
    assert(pred != nullptr);
    auto rhs = std::move(pred);
    assert(pred == nullptr);
    pred = std::make_unique<OrPredicate>(std::move(lhs),std::move(rhs));
    assert(pred != nullptr);
}

std::unique_ptr<joda::query::Predicate> joda::query::CopyPredicateVisitor::getPredicate() {
    return std::move(pred);
}
void joda::query::CopyPredicateVisitor::visit(EqualizePredicate *e) {
    assert(pred == nullptr);
    pred = std::move(e->duplicate());
    assert(pred != nullptr);
}
void joda::query::CopyPredicateVisitor::visit(joda::query::ComparePredicate *e) {
    assert(pred == nullptr);
    pred = std::move(e->duplicate());
    assert(pred != nullptr);
}

void joda::query::CopyPredicateVisitor::visit(ValToPredicate *e) {
    assert(pred == nullptr);
    pred = std::move(e->duplicate());
    assert(pred != nullptr);
}
