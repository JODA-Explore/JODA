//
// Created by Nico on 28/02/2019.
//

#include <gtest/gtest.h>
#include <joda/document/RapidJsonDocument.h>
#include <joda/document/TemporaryOrigin.h>
#include <joda/misc/RJFwd.h>
#include <joda/query/aggregation/AttributeStatAggregator.h>
#include <joda/query/aggregation/CountAggregator.h>
#include <joda/query/aggregation/DistinctAggregator.h>
#include <joda/query/aggregation/IAggregator.h>
#include <joda/query/aggregation/NumberAggregator.h>
#include <joda/query/aggregation/HistogramAggregator.h>
#include <joda/query/values/IValueProvider.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "../../values/test/IValueTestHelper.h"

class AggregatorTest : public ::testing::Test {
 protected:
  typedef std::vector<std::unique_ptr<joda::query::IValueProvider>> Params;

  std::vector<RapidJsonDocument> getDocs(std::vector<std::string> strs,
                                         RJMemoryPoolAlloc& alloc) {
    std::vector<RapidJsonDocument> docs;
    for (const auto& str : strs) {
      auto rjdoc = std::make_unique<RJDocument>();
      rjdoc->Parse(str.c_str());
      DCHECK(!rjdoc->HasParseError()) << rjdoc->GetParseError();
      docs.emplace_back(0, std::move(rjdoc),
                        std::make_unique<TemporaryOrigin>());
    }
    return docs;
  }

  std::string rjvalToString(RJValue& val) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    val.Accept(writer);
    return buffer.GetString();
  }

  std::string rjvalToString(RJDocument& val) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    val.Accept(writer);
    return buffer.GetString();
  }

  void aggregate(const std::vector<RapidJsonDocument>& docs,
                 RJMemoryPoolAlloc& alloc,
                 std::unique_ptr<joda::query::IAggregator>& agg) {
    for (const auto& doc : docs) {
      agg->accumulate(doc, alloc);
    }
  }

  void testAgg(const std::vector<RapidJsonDocument>& docs,
               RJMemoryPoolAlloc& alloc, std::string res,
               std::unique_ptr<joda::query::IAggregator>& agg) {
    aggregate(docs, alloc, agg);
    auto aggVal = agg->terminate(alloc);
    RJDocument resVal(&alloc);
    resVal.Parse(res.c_str());
    DCHECK(!resVal.HasParseError());

    EXPECT_TRUE(aggVal == resVal) << "Agg: " << rjvalToString(aggVal)
                                  << " ; Expected: " << rjvalToString(resVal);
  }

  void testAgg(RJMemoryPoolAlloc& alloc, std::string res,
               std::unique_ptr<joda::query::IAggregator>& agg) {
    std::vector<RapidJsonDocument> docs;
    return testAgg(docs, alloc, res, agg);
  }

  void checkDuplicate(std::unique_ptr<joda::query::IAggregator>& agg) {
    auto dupe = agg->duplicate();
    EXPECT_STREQ(agg->getName().c_str(), dupe->getName().c_str());
    EXPECT_STREQ(agg->toString().c_str(), dupe->toString().c_str());
  }

  ::testing::AssertionResult testArrayAgg(
      const std::vector<RapidJsonDocument>& docs, RJMemoryPoolAlloc& alloc,
      std::vector<std::unique_ptr<joda::query::IValueProvider>>& vals,
      std::unique_ptr<joda::query::IAggregator>& agg) {
    aggregate(docs, alloc, agg);
    auto aggVal = agg->terminate(alloc);
    if (!aggVal.IsArray()) {
      return ::testing::AssertionFailure() << " not an array";
    }
    if (aggVal.Size() != vals.size()) {
      return ::testing::AssertionFailure()
             << " arrays do not have the same size";
    }
    for (const auto& val : vals) {
      DCHECK(val->isConst() && val->isAtom());
      auto v = val->getAtomValue(docs.front(), alloc);
      bool contains = false;
      for (const auto& arrElt : aggVal.GetArray()) {
        if (v == arrElt) {
          contains = true;
          break;
        }
      }
      if (!contains) {
        return ::testing::AssertionFailure()
               << rjvalToString(v) << " not in array " << rjvalToString(aggVal);
      }
    }
    return ::testing::AssertionSuccess();
  }
};

TEST_F(AggregatorTest, CountAggregator) {
  SCOPED_TRACE("CountAggregator");

  RJMemoryPoolAlloc alloc;
  auto docs = getDocs(
      {
          R"(1)",
          R"(2)",
          R"(3)",
          R"(4)",
          R"(5)",
      },
      alloc);

  std::unique_ptr<joda::query::IAggregator> agg;
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
  EXPECT_NO_THROW(agg = std::make_unique<joda::query::CountAggregator>(
                      "/agg", std::move(p)));

  // Duplicate
  checkDuplicate(agg);

  // Function
  testAgg(docs, alloc, "5", agg);

  // Merge
  {
    SCOPED_TRACE("Merge");
    std::unique_ptr<joda::query::IAggregator> agg2;
    EXPECT_NO_THROW(agg2 = agg->duplicate());
    testAgg(docs, alloc, "5", agg2);
    EXPECT_NO_FATAL_FAILURE(agg->merge(agg2.get()));
    testAgg(alloc, "10", agg);
  }

  // Nulltest
  {
    SCOPED_TRACE("Null");
    auto ptr = agg->duplicate();
    testAgg(alloc, "0", ptr);
  }
}

TEST_F(AggregatorTest, SumAggregator) {
  SCOPED_TRACE("SumAggregator");

  RJMemoryPoolAlloc alloc;
  auto docs = getDocs(
      {
          R"(1)",
          R"(2)",
          R"(3)",
          R"(4)",
          R"(5)",
      },
      alloc);

  std::unique_ptr<joda::query::IAggregator> agg;
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
  EXPECT_NO_THROW(
      agg = std::make_unique<joda::query::SumAggregator>("/agg", std::move(p)));

  // Duplicate
  checkDuplicate(agg);

  // Function
  testAgg(docs, alloc, "15", agg);

  // Merge
  {
    SCOPED_TRACE("Merge");
    std::unique_ptr<joda::query::IAggregator> agg2;
    EXPECT_NO_THROW(agg2 = agg->duplicate());
    docs = getDocs({R"(-1)", R"(-2)", R"(-3)", R"(-4)"}, alloc);
    testAgg(docs, alloc, "-10", agg2);
    EXPECT_NO_FATAL_FAILURE(agg->merge(agg2.get()));
    testAgg(alloc, "5", agg);
  }

  // Nulltest
  {
    SCOPED_TRACE("Null");
    auto ptr = agg->duplicate();
    testAgg(alloc, "null", ptr);
  }
}

TEST_F(AggregatorTest, AverageAggregator) {
  SCOPED_TRACE("AverageAggregator");

  RJMemoryPoolAlloc alloc;
  auto docs = getDocs(
      {
          R"(1)",
          R"(2)",
          R"(3)",
          R"(4)",
          R"(5)",
      },
      alloc);

  std::unique_ptr<joda::query::IAggregator> agg;
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
  EXPECT_NO_THROW(agg = std::make_unique<joda::query::AverageAggregator>(
                      "/agg", std::move(p)));

  // Duplicate
  checkDuplicate(agg);

  // Function
  testAgg(docs, alloc, "3", agg);

  // Merge
  {
    SCOPED_TRACE("Merge");
    std::unique_ptr<joda::query::IAggregator> agg2;
    EXPECT_NO_THROW(agg2 = agg->duplicate());
    docs = getDocs({R"(-1)", R"(-2)", R"(-3)"}, alloc);
    testAgg(docs, alloc, "-2", agg2);
    EXPECT_NO_FATAL_FAILURE(agg->merge(agg2.get()));
    testAgg(alloc, "1.125", agg);
  }

  // Nulltest
  {
    SCOPED_TRACE("Null");
    auto ptr = agg->duplicate();
    testAgg(alloc, "null", ptr);
  }
}

TEST_F(AggregatorTest, MaxAggregator) {
  SCOPED_TRACE("MaxAggregator");

  RJMemoryPoolAlloc alloc;
  auto docs = getDocs(
      {
          R"(1)",
          R"(2)",
          R"(3)",
          R"(4)",
          R"(5)",
      },
      alloc);

  std::unique_ptr<joda::query::IAggregator> agg;
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
  EXPECT_NO_THROW(
      agg = std::make_unique<joda::query::MaxAggregator>("/agg", std::move(p)));

  // Duplicate
  checkDuplicate(agg);

  // Function
  testAgg(docs, alloc, "5", agg);

  // Merge
  {
    SCOPED_TRACE("Merge");
    std::unique_ptr<joda::query::IAggregator> agg2;
    EXPECT_NO_THROW(agg2 = agg->duplicate());
    docs = getDocs({R"(-1)", R"(-2)", R"(-3)"}, alloc);
    testAgg(docs, alloc, "-1", agg2);
    EXPECT_NO_FATAL_FAILURE(agg->merge(agg2.get()));
    testAgg(alloc, "5", agg);
  }

  // Nulltest
  {
    SCOPED_TRACE("Null");
    auto ptr = agg->duplicate();
    testAgg(alloc, "null", ptr);
  }
}

TEST_F(AggregatorTest, MinAggregator) {
  SCOPED_TRACE("MinAggregator_Min");

  RJMemoryPoolAlloc alloc;
  auto docs = getDocs(
      {
          R"(1)",
          R"(2)",
          R"(3)",
          R"(4)",
          R"(5)",
      },
      alloc);

  std::unique_ptr<joda::query::IAggregator> agg;
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
  EXPECT_NO_THROW(
      agg = std::make_unique<joda::query::MinAggregator>("/agg", std::move(p)));

  // Duplicate
  checkDuplicate(agg);

  // Function
  testAgg(docs, alloc, "1", agg);

  // Merge
  {
    SCOPED_TRACE("Merge");
    std::unique_ptr<joda::query::IAggregator> agg2;
    EXPECT_NO_THROW(agg2 = agg->duplicate());
    docs = getDocs({R"(-1)", R"(-2)", R"(-3)"}, alloc);
    testAgg(docs, alloc, "-3", agg2);
    EXPECT_NO_FATAL_FAILURE(agg->merge(agg2.get()));
    testAgg(alloc, "-3", agg);
  }

  // Nulltest
  {
    SCOPED_TRACE("Null");
    auto ptr = agg->duplicate();
    testAgg(alloc, "null", ptr);
  }
}

TEST_F(AggregatorTest, DistinctAggregator) {
  SCOPED_TRACE("DistinctAggregator");

  RJMemoryPoolAlloc alloc;
  auto docs = getDocs(
      {
          R"(1)",
          R"(1)",
          R"("str")",
          R"("str")",
          R"(true)",
      },
      alloc);

  std::unique_ptr<joda::query::IAggregator> agg;
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
  EXPECT_NO_THROW(agg = std::make_unique<joda::query::DistinctAggregator>(
                      "/agg", std::move(p)));

  // Duplicate
  checkDuplicate(agg);

  // Function
  Params res = {};
  IValueTestHelper::param(res, IValueTestHelper::getStringVal("str"),
                          IValueTestHelper::getNumVal(1.0),
                          IValueTestHelper::getBoolVal(true));
  testArrayAgg(docs, alloc, res, agg);

  // Merge
  {
    SCOPED_TRACE("Merge");
    std::unique_ptr<joda::query::IAggregator> agg2;
    EXPECT_NO_THROW(agg2 = agg->duplicate());
    docs = getDocs(
        {
            R"(-1)",
            R"(1)",
            R"(false)",
            R"("str2")",
        },
        alloc);
    res.clear();
    IValueTestHelper::param(res, IValueTestHelper::getStringVal("str2"),
                            IValueTestHelper::getNumVal(1.0),
                            IValueTestHelper::getNumVal(-1.0),
                            IValueTestHelper::getBoolVal(false));
    testArrayAgg(docs, alloc, res, agg2);

    EXPECT_NO_FATAL_FAILURE(agg->merge(agg2.get()));
    EXPECT_NO_FATAL_FAILURE(agg->merge(agg2.get()));
    res.clear();
    IValueTestHelper::param(res, IValueTestHelper::getStringVal("str"),
                            IValueTestHelper::getStringVal("str2"),
                            IValueTestHelper::getNumVal(1.0),
                            IValueTestHelper::getNumVal(-1.0),
                            IValueTestHelper::getBoolVal(false),
                            IValueTestHelper::getBoolVal(true));
    testArrayAgg(docs, alloc, res, agg);
  }

  // Nulltest
  {
    SCOPED_TRACE("Null");
    auto ptr = agg->duplicate();
    testAgg(alloc, "[]", ptr);
  }
}

TEST_F(AggregatorTest, AttributeStatAggregatorSingle) {
  SCOPED_TRACE("AttributeStatAggregator");
  RJMemoryPoolAlloc alloc;
  auto docs = getDocs(
      {R"(1)", R"("str")", R"(true)", R"(null)", R"({"str":"str"})", R"([1])"},
      alloc);

  std::unique_ptr<joda::query::IAggregator> agg;
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
  EXPECT_NO_THROW(agg = std::make_unique<joda::query::AttributeStatAggregator>(
                      "/agg", std::move(p)));

  // Nulltest
  {
    SCOPED_TRACE("Null");
    auto ptr = agg->duplicate();
    testAgg(
        {}, alloc,
        R"({"Count_Total":0,"Count_Object":0,"Count_Array":0,"Count_Null":0,"Count_Boolean":0,"Count_True":0,"Count_False":0,"Count_String":0,"Count_Int":0,"Count_Float":0,"Count_Number":0})",
        ptr);
  }

  // Duplicate
  checkDuplicate(agg);

  // Function
  testAgg(
      docs, alloc,
      R"({"Count_Total":6,"Count_Object":1,"Min_Member":1,"Max_Member":1,"Count_Array":1,"Min_Size":1,"Max_Size":1,"Count_Null":1,"Count_Boolean":1,"Count_True":1,"Count_False":0,"Count_String":1,"Min_StrSize":3,"Max_StrSize":3,"Count_Int":1,"Min_Int":1,"Max_Int":1,"Count_Float":0,"Count_Number":1,"Children":[{"Key":"str","Count_Total":1,"Count_Object":0,"Count_Array":0,"Count_Null":0,"Count_Boolean":0,"Count_True":0,"Count_False":0,"Count_String":1,"Min_StrSize":3,"Max_StrSize":3,"Count_Int":0,"Count_Float":0,"Count_Number":0}],"Array_Items":[{"Key":"0","Count_Total":1,"Count_Object":0,"Count_Array":0,"Count_Null":0,"Count_Boolean":0,"Count_True":0,"Count_False":0,"Count_String":0,"Count_Int":1,"Min_Int":1,"Max_Int":1,"Count_Float":0,"Count_Number":1}]} )",
      agg);

  // Second
  {
    SCOPED_TRACE("Second");
    std::unique_ptr<joda::query::IAggregator> agg2;
    EXPECT_NO_THROW(agg2 = agg->duplicate());
    docs = getDocs({R"(null)", R"({"num":1})"}, alloc);

    testAgg(
        docs, alloc,
        R"({"Count_Total":2,"Count_Object":1,"Min_Member":1,"Max_Member":1,"Count_Array":0,"Count_Null":1,"Count_Boolean":0,"Count_True":0,"Count_False":0,"Count_String":0,"Count_Int":0,"Count_Float":0,"Count_Number":0,"Children":[{"Key":"num","Count_Total":1,"Count_Object":0,"Count_Array":0,"Count_Null":0,"Count_Boolean":0,"Count_True":0,"Count_False":0,"Count_String":0,"Count_Int":1,"Min_Int":1,"Max_Int":1,"Count_Float":0,"Count_Number":1}]})",
        agg2);
  }
}

TEST_F(AggregatorTest, AttributeStatAggregatorMerge) {
  RJMemoryPoolAlloc alloc;
  auto docs = getDocs(
      {R"(1)", R"("str")", R"(true)", R"(null)", R"({"str":"str"})", R"([1])"},
      alloc);

  std::unique_ptr<joda::query::IAggregator> agg;
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
  EXPECT_NO_THROW(agg = std::make_unique<joda::query::AttributeStatAggregator>(
                      "/agg", std::move(p)));
  // Function
  aggregate(docs, alloc, agg);

  // Merge
  std::unique_ptr<joda::query::IAggregator> agg2;
  EXPECT_NO_THROW(agg2 = agg->duplicate());
  docs = getDocs({R"(null)", R"({"num":1})"}, alloc);

  aggregate(docs, alloc, agg2);

  EXPECT_NO_FATAL_FAILURE(agg->merge(agg2.get()));

  testAgg(
      {}, alloc,
      R"({"Count_Total":8,"Count_Object":2,"Min_Member":1,"Max_Member":1,"Count_Array":1,"Min_Size":1,"Max_Size":1,"Count_Null":2,"Count_Boolean":1,"Count_True":1,"Count_False":0,"Count_String":1,"Min_StrSize":3,"Max_StrSize":3,"Count_Int":1,"Min_Int":1,"Max_Int":1,"Count_Float":0,"Count_Number":1,"Children":[{"Key":"str","Count_Total":1,"Count_Object":0,"Count_Array":0,"Count_Null":0,"Count_Boolean":0,"Count_True":0,"Count_False":0,"Count_String":1,"Min_StrSize":3,"Max_StrSize":3,"Count_Int":0,"Count_Float":0,"Count_Number":0},{"Key":"num","Count_Total":1,"Count_Object":0,"Count_Array":0,"Count_Null":0,"Count_Boolean":0,"Count_True":0,"Count_False":0,"Count_String":0,"Count_Int":1,"Min_Int":1,"Max_Int":1,"Count_Float":0,"Count_Number":1}],"Array_Items":[{"Key":"0","Count_Total":1,"Count_Object":0,"Count_Array":0,"Count_Null":0,"Count_Boolean":0,"Count_True":0,"Count_False":0,"Count_String":0,"Count_Int":1,"Min_Int":1,"Max_Int":1,"Count_Float":0,"Count_Number":1}]})",
      agg);
}



TEST_F(AggregatorTest, HistogramAggregator) {
  SCOPED_TRACE("HistogramAggregator");
  RJMemoryPoolAlloc alloc;
  auto docs = getDocs(
      {R"(1)", R"(2)", R"(3)", R"(1.5)", R"(3.5)"},
      alloc);

  std::unique_ptr<joda::query::IAggregator> agg;
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
  IValueTestHelper::param(p, IValueTestHelper::getNumVal((u_int64_t)3));
  IValueTestHelper::param(p, IValueTestHelper::getNumVal((u_int64_t)1));
  IValueTestHelper::param(p, IValueTestHelper::getNumVal((u_int64_t)4));
  EXPECT_NO_THROW(agg = std::make_unique<joda::query::HistogramAggregator>(
                      "/agg", std::move(p)));

  // Nulltest
  {
    SCOPED_TRACE("Null");
    auto ptr = agg->duplicate();
    testAgg(
        {}, alloc,
        R"([{"lower":1.0,"upper":2.0,"value":0},{"lower":2.0,"upper":3.0,"value":0},{"lower":3.0,"upper":4.0,"value":0}])",
        ptr);
  }

  // Duplicate
  checkDuplicate(agg);

  // Function
  testAgg(
      docs, alloc,
      R"([{"lower":1.0,"upper":2.0,"value":2},{"lower":2.0,"upper":3.0,"value":1},{"lower":3.0,"upper":4.0,"value":2}])",
      agg);

  // Overflow
  {
    SCOPED_TRACE("Overflow");
    std::unique_ptr<joda::query::IAggregator> agg2;
    EXPECT_NO_THROW(agg2 = agg->duplicate());
     auto docs = getDocs(
      {R"(-1)", R"(2)", R"(4)", R"(7.5)"},
      alloc);

    testAgg(
        docs, alloc,
        R"([{"upper":1.0,"value":1},{"lower":1.0,"upper":2.0,"value":0},{"lower":2.0,"upper":3.0,"value":1},{"lower":3.0,"upper":4.0,"value":0},{"lower":4.0,"value":2}])",
        agg2);
  }

  // Wrong Types
  {
    SCOPED_TRACE("Wrong Types");
    std::unique_ptr<joda::query::IAggregator> agg3;
    EXPECT_NO_THROW(agg3 = agg->duplicate());
     auto docs = getDocs(
      {R"("string")", R"(true)", R"(false)", R"({})", R"([1])", R"(null)"},
      alloc);

    testAgg(
        docs, alloc,
        R"([{"lower":1.0,"upper":2.0,"value":0},{"lower":2.0,"upper":3.0,"value":0},{"lower":3.0,"upper":4.0,"value":0}])",
        agg3);
  }

   // Merge
  {
    SCOPED_TRACE("Merge");
    std::unique_ptr<joda::query::IAggregator> agg4;
    EXPECT_NO_THROW(agg4 = agg->duplicate());
    std::unique_ptr<joda::query::IAggregator> agg5;
    EXPECT_NO_THROW(agg5 = agg->duplicate());
    auto docs1 = getDocs(
      {R"(1)", R"(2)", R"(3)", R"(1.5)", R"(3.5)"},
      alloc);
    auto docs2 = getDocs(
      {R"(-1)", R"(2.5)", R"(4)"},
      alloc);

    aggregate(docs1, alloc, agg4);
    aggregate(docs2, alloc, agg5);
    EXPECT_NO_FATAL_FAILURE(agg4->merge(agg5.get()));

    testAgg(
      {}, alloc,
      R"([{"upper":1.0,"value":1},{"lower":1.0,"upper":2.0,"value":2},{"lower":2.0,"upper":3.0,"value":2},{"lower":3.0,"upper":4.0,"value":2},{"lower":4.0,"value":1}])",
      agg4);
  }
}