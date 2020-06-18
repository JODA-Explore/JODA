//
// Created by Nico on 20/05/2019.
//

#include <gtest/gtest.h>
#include <joda/query/aggregation/AttributeStatAggregator.h>
#include <joda/query/aggregation/DistinctAggregator.h>
#include <joda/query/aggregation/NumberAggregator.h>
#include <joda/queryparsing/QueryParser.h>
#include <joda/query/values/PointerProvider.h>
#include <joda/query/aggregation/CountAggregator.h>

template<typename T>
class AggregatorParsingTest : public testing::Test {
 public:
  std::unique_ptr<T> referenceAgg;

  void SetUp() override {
    std::string args = "/agg";
    std::vector<std::unique_ptr<joda::query::IValueProvider>> x;
    x.emplace_back(std::make_unique<joda::query::PointerProvider>("/"));
    referenceAgg = std::make_unique<T>(args, std::move(x));
  }

  void TearDown() override { referenceAgg = nullptr; }

};

using AggregatorTypes = testing::Types<
    joda::query::AttributeStatAggregator,
    joda::query::DistinctAggregator,
    joda::query::CountAggregator,
    joda::query::NumberAggregator<joda::query::MinAggregatorFunc>,
    joda::query::NumberAggregator<joda::query::MaxAggregatorFunc>,
    joda::query::NumberAggregator<joda::query::SumAggregatorFunc>,
    joda::query::NumberAggregator<joda::query::AverageAggregatorFunc>
>;

TYPED_TEST_CASE(AggregatorParsingTest, AggregatorTypes);

TYPED_TEST(AggregatorParsingTest, Parses) {
  joda::queryparsing::QueryParser qp;
  std::string name = TypeParam::getName_();
  std::string qStr = "LOAD A AGG ('/agg':";
  qStr += name;
  qStr += "('/'))";
  auto q = qp.parse(qStr);
  if (q == nullptr) {
    EXPECT_FALSE(true) << qp.getLastError();
  } else {
    auto &aggs = q->getAggregators();
    EXPECT_EQ(aggs.size(), 1);
    EXPECT_TRUE(aggs.front() != nullptr);
    EXPECT_STRCASEEQ(name.c_str(), aggs.front()->getName().c_str());
    EXPECT_STRCASEEQ(this->referenceAgg->toString().c_str(), aggs.front()->toString().c_str());

    auto *ptr = dynamic_cast<TypeParam *>(aggs.front().get());
    EXPECT_TRUE(ptr != nullptr);
  }
}