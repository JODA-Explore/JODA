#pragma once

#include <joda/indexing/AdaptiveIndexQuery.h>
#include <joda/query/values/TruthyFalsyProvider.h>
#include <joda/query/values/EqualityProvider.h>
#include <joda/query/values/ComparisonProvider.h>

class AdaptiveQueryCreator {
public:
    static std::unique_ptr<AdaptiveIndexQuery> createQuery(joda::query::LessProvider *e);
    static std::unique_ptr<AdaptiveIndexQuery> createQuery(joda::query::GreaterProvider *e);
    static std::unique_ptr<AdaptiveIndexQuery> createQuery(joda::query::LessEqualProvider *e);
    static std::unique_ptr<AdaptiveIndexQuery> createQuery(joda::query::GreaterEqualProvider *e);
    static std::unique_ptr<AdaptiveIndexQuery> createQuery(joda::query::EqualProvider *e);
    static std::unique_ptr<AdaptiveIndexQuery> createQuery(joda::query::UnequalProvider *e);
    static std::unique_ptr<AdaptiveIndexQuery> createQuery(joda::query::IValueProvider *e);

private:
    static std::string toJsonPropPath(const std::unique_ptr<joda::query::IValueProvider> &valueProvider);
    static RJValue getValue(const std::unique_ptr<joda::query::IValueProvider> &valueProvider);
    static std::unique_ptr<AdaptiveIndexQuery> createQueryFromEQ(AdaptiveIndexQuery::COMPARISON_TYPE comparisonType, const std::unique_ptr<joda::query::IValueProvider> &lhs, const std::unique_ptr<joda::query::IValueProvider> &rhs);
    static std::unique_ptr<AdaptiveIndexQuery> createQueryFromComp(AdaptiveIndexQuery::COMPARISON_TYPE comparisonType, const std::unique_ptr<joda::query::IValueProvider> &lhs, const std::unique_ptr<joda::query::IValueProvider> &rhs);
};
