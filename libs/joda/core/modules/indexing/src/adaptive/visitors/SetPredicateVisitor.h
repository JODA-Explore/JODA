#pragma once

#include <joda/query/values/BinaryBoolProvider.h>
#include <joda/query/values/ComparisonProvider.h>
#include <joda/query/values/EqualityProvider.h>
#include <joda/query/values/NotProvider.h>
#include <joda/query/values/TruthyFalsyProvider.h>

class SetPredicateVisitor {
 public:
  SetPredicateVisitor(bool value);

  void visit(joda::query::IValueProvider *e);
  void visit(joda::query::AndProvider *e);
  void visit(joda::query::NotProvider *e);
  void visit(joda::query::OrProvider *e);
  void visit(joda::query::LessProvider *e);
  void visit(joda::query::GreaterProvider *e);
  void visit(joda::query::LessEqualProvider *e);
  void visit(joda::query::GreaterEqualProvider *e);
  void visit(joda::query::EqualProvider *e);
  void visit(joda::query::UnequalProvider *e);

  std::unique_ptr<joda::query::IValueProvider> getPred();

 protected:
  bool changed = false;
  joda::query::IValueProvider *targetPredicate;
  bool value;
  std::unique_ptr<joda::query::IValueProvider> pred;
};