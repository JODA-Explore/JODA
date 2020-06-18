//
// Created by Nico on 20/05/2019.
//


#include <gtest/gtest.h>
#include <joda/query/values/IValueProvider.h>
#include <joda/query/values/PointerProvider.h>
#include <joda/query/aggregation/NumberAggregator.h>
#include <joda/query/values/AtomProvider.h>
#include <numeric>
#include <cmath>

template<typename T>
class NumberAggregatorTest : public testing::Test {
 public:
  std::unique_ptr<joda::query::NumberAggregator<T>> referenceAgg;

  void SetUp() override {
    std::string args = "/agg";
    std::vector<std::unique_ptr<joda::query::IValueProvider>> x;
    x.emplace_back(std::make_unique<joda::query::PointerProvider>("/"));
    referenceAgg = std::make_unique<joda::query::NumberAggregator<T>>(args, std::move(x));
  }

  void TearDown() override { referenceAgg = nullptr; }

  double aggVector(const std::vector<double> &vec) {
    EXPECT_TRUE(false) << "Not specialized";
    return std::nan("");
  }

  std::vector<std::vector<double>> nums{
      {1, 2, 3, 4, 5, 6, 7, 8, 9},
      {5},
      {-5, -1, -5, -5},
      {},
      {1, -6, 8},
  };

};

using AggregatorFuncTypes = testing::Types<
    joda::query::MinAggregatorFunc,
    joda::query::MaxAggregatorFunc,
    joda::query::SumAggregatorFunc,
    joda::query::AverageAggregatorFunc
>;

template<>
double NumberAggregatorTest<joda::query::MinAggregatorFunc>::aggVector(const std::vector<double> &vec) {
  auto iterator = std::min_element(vec.begin(), vec.end());
  if (iterator != vec.end())
    return *iterator;
  else return 0;
}

template<>
double NumberAggregatorTest<joda::query::MaxAggregatorFunc>::aggVector(const std::vector<double> &vec) {
  auto iterator = std::max_element(vec.begin(), vec.end());
  if (iterator != vec.end())
    return *iterator;
  else return 0;
}

template<>
double NumberAggregatorTest<joda::query::SumAggregatorFunc>::aggVector(const std::vector<double> &vec) {
  return std::accumulate(vec.begin(), vec.end(), (double) 0);
}


template<>
double NumberAggregatorTest<joda::query::AverageAggregatorFunc>::aggVector(const std::vector<double> &vec) {
  if (!vec.empty())
    return std::accumulate(vec.begin(), vec.end(), (double) 0) / vec.size();
  return std::nan("");
}

TYPED_TEST_CASE(NumberAggregatorTest, AggregatorFuncTypes);

TYPED_TEST(NumberAggregatorTest, Construct) {
  std::string toPtr = "/toptr";
  auto fromPtr = "/from/ptr";
  std::vector<std::unique_ptr<joda::query::IValueProvider>> arguments;
  arguments.emplace_back(std::make_unique<joda::query::PointerProvider>(fromPtr));
  std::unique_ptr<joda::query::IAggregator> agg;
  EXPECT_NO_THROW(agg = std::make_unique<joda::query::NumberAggregator<TypeParam>>(toPtr, std::move(arguments)););
  EXPECT_STRCASEEQ(toPtr.c_str(), agg->getDestPointer().c_str());
}

TYPED_TEST(NumberAggregatorTest, TooFewParams) {
  std::string toPtr = "/toptr";
  auto fromPtr = "/from/ptr";
  std::vector<std::unique_ptr<joda::query::IValueProvider>> arguments;
  std::unique_ptr<joda::query::IAggregator> agg;
  EXPECT_THROW(agg = std::make_unique<joda::query::NumberAggregator<TypeParam>>(toPtr, std::move(arguments));,
               joda::query::WrongParameterCountException);
}

TYPED_TEST(NumberAggregatorTest, SecondParamWrongTypeParams) {
  std::string toPtr = "/toptr";
  auto fromPtr = "/from/ptr";
  std::vector<std::unique_ptr<joda::query::IValueProvider>> arguments;
  arguments.emplace_back(std::make_unique<joda::query::PointerProvider>(fromPtr));
  arguments.emplace_back(std::make_unique<joda::query::StringProvider>("tmp"));
  std::unique_ptr<joda::query::IAggregator> agg;
  EXPECT_THROW(agg = std::make_unique<joda::query::NumberAggregator<TypeParam>>(toPtr, std::move(arguments));,
               joda::query::WrongParameterTypeException);
}

TYPED_TEST(NumberAggregatorTest, SecondParamCorrectTypeParams_true) {
  std::string toPtr = "/toptr";
  auto fromPtr = "/from/ptr";
  std::vector<std::unique_ptr<joda::query::IValueProvider>> arguments;
  arguments.emplace_back(std::make_unique<joda::query::PointerProvider>(fromPtr));
  arguments.emplace_back(std::make_unique<joda::query::BoolProvider>(true));
  std::unique_ptr<joda::query::IAggregator> agg;
  EXPECT_NO_THROW(agg = std::make_unique<joda::query::NumberAggregator<TypeParam>>(toPtr, std::move(arguments)););
}

TYPED_TEST(NumberAggregatorTest, SecondParamCorrectTypeParams_false) {
  std::string toPtr = "/toptr";
  auto fromPtr = "/from/ptr";
  std::vector<std::unique_ptr<joda::query::IValueProvider>> arguments;
  arguments.emplace_back(std::make_unique<joda::query::PointerProvider>(fromPtr));
  arguments.emplace_back(std::make_unique<joda::query::BoolProvider>(true));
  std::unique_ptr<joda::query::IAggregator> agg;
  EXPECT_NO_THROW(agg = std::make_unique<joda::query::NumberAggregator<TypeParam>>(toPtr, std::move(arguments)););
}

TYPED_TEST(NumberAggregatorTest, TooManyParams) {
  std::string toPtr = "/toptr";
  auto fromPtr = "/from/ptr";
  std::vector<std::unique_ptr<joda::query::IValueProvider>> arguments;
  arguments.emplace_back(std::make_unique<joda::query::PointerProvider>(fromPtr));
  arguments.emplace_back(std::make_unique<joda::query::BoolProvider>(true));
  arguments.emplace_back(std::make_unique<joda::query::StringProvider>("tmp"));
  std::unique_ptr<joda::query::IAggregator> agg;
  EXPECT_THROW(agg = std::make_unique<joda::query::NumberAggregator<TypeParam>>(toPtr, std::move(arguments));,
               joda::query::WrongParameterCountException);
}

TYPED_TEST(NumberAggregatorTest, Duplicate) {
  auto dupe = this->referenceAgg->duplicate();
  EXPECT_STREQ(this->referenceAgg->getName().c_str(), dupe->getName().c_str());
  EXPECT_STREQ(this->referenceAgg->toString().c_str(), dupe->toString().c_str());
}

TYPED_TEST(NumberAggregatorTest, Name) {
  EXPECT_STREQ(TypeParam::name, this->referenceAgg->getName().c_str());
  EXPECT_STREQ(TypeParam::name, this->referenceAgg->getName_());
}

TYPED_TEST(NumberAggregatorTest, NullTest) {
  RJMemoryPoolAlloc alloc;
  auto val = this->referenceAgg->terminate(alloc);
  EXPECT_TRUE(val.IsNull());
}

TYPED_TEST(NumberAggregatorTest, TestResults) {
  int i = 0;
  for (const auto &numVec : this->nums) {
    SCOPED_TRACE(i);
    auto res = this->aggVector(numVec);
    typename TypeParam::AggRep rep{};
    bool first = true;
    for (const auto &num : numVec) {
      if (first) {
        TypeParam::aggregateFirst(rep, num);
        first = false;
      } else {
        TypeParam::aggregate(rep, num);
      }
    }
    i++;
    if (std::isnan(res)) {
      EXPECT_TRUE(std::isnan(TypeParam::getDouble(rep)));
    } else {
      EXPECT_DOUBLE_EQ(res, TypeParam::getDouble(rep));
    }
  }
}


