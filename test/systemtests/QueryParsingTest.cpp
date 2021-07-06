//
// Created by nico on 11/10/17.
//
#include "gtest/gtest.h"

#include <joda/export/FileExport.h>
#include <joda/export/StorageExport.h>
#include <joda/parser/JSONFileDirectorySource.h>
#include <joda/parser/JSONFileSource.h>
#include <joda/query/Query.h>
#include <joda/query/aggregation/IAggregator.h>
#include <joda/query/predicate/PredFWD.h>
#include <joda/query/predicate/Predicate.h>
#include <joda/query/predicate/ToStringVisitor.h>
#include <joda/query/project/DeletePointerProjector.h>
#include <joda/query/project/IProjector.h>
#include <joda/query/project/ISetProjector.h>
#include <joda/query/project/PointerCopyProject.h>
#include <joda/query/project/ValueProviderProjector.h>
#include <joda/query/values/AtomProvider.h>
#include <joda/query/values/ConstantNumber.h>
#include <joda/query/values/PointerProvider.h>
#include <joda/queryparsing/QueryParser.h>
#include <joda/storage/JSONStorage.h>
#include <tao/pegtl/parse_error.hpp>

class QueryParsingTest : public ::testing::Test {
 protected:
  const std::string default_storeVar = {};
  const std::string default_deleteVar = {};
  const std::unique_ptr<joda::query::Predicate> default_choosePred =
      std::make_unique<joda::query::ValToPredicate>(true);
  const std::vector<std::unique_ptr<joda::docparsing::IImportSource>>
      default_importSources = {};
  const std::unique_ptr<IExportDestination> default_exportDestination = nullptr;
  const std::vector<std::unique_ptr<joda::query::IProjector>> default_proj = {};
  const std::vector<std::unique_ptr<joda::query::ISetProjector>>
      default_setproj = {};
  const std::vector<std::unique_ptr<joda::query::IAggregator>> default_agg = {};

  std::shared_ptr<joda::query::Query> parseQuery(const std::string& str,
                                                 bool expect = true) {
    joda::queryparsing::QueryParser qp;
    auto q = qp.parse(str);
    if (q == nullptr) {
      EXPECT_FALSE(expect) << qp.getLastError();
      return nullptr;
    } else {
      EXPECT_TRUE(expect);
      return q;
    }
  }

  void equalImportSource(
      const std::unique_ptr<joda::docparsing::IImportSource>& lhs,
      const std::unique_ptr<joda::docparsing::IImportSource>& rhs) {
    if (lhs != nullptr && rhs != nullptr) {
      auto lhsString = lhs->toString();
      auto rhsString = rhs->toString();

      EXPECT_STREQ(lhsString.c_str(), rhsString.c_str());
      return;
    }
    EXPECT_TRUE(false);
  }

  std::unique_ptr<joda::query::Predicate> IValToPred(
      std::unique_ptr<joda::query::IValueProvider>&& val) {
    return std::make_unique<joda::query::ValToPredicate>(std::move(val));
  }

  void checkEqual(const std::shared_ptr<joda::query::Predicate>& toTest,
                  const std::unique_ptr<joda::query::Predicate>& control) {
    joda::query::ToStringVisitor stringify;
    toTest->accept(stringify);
    auto testStr = stringify.popString();
    control->accept(stringify);
    auto contrStr = stringify.popString();
    EXPECT_STREQ(testStr.c_str(), contrStr.c_str());
  }

  void equals(
      std::shared_ptr<joda::query::Query>& q, const std::string& loadVar,
      const std::vector<std::unique_ptr<joda::docparsing::IImportSource>>&
          importSources,
      const std::unique_ptr<IExportDestination>& exportDestination,
      const std::string& delete_var,
      const std::unique_ptr<joda::query::Predicate>& choosePred,
      const std::vector<std::unique_ptr<joda::query::IProjector>>& proj,
      const std::vector<std::unique_ptr<joda::query::ISetProjector>>& setproj,
      const std::vector<std::unique_ptr<joda::query::IAggregator>>& agg) {
    // LOAD X
    EXPECT_STREQ(loadVar.c_str(), q->getLoad().c_str());

    // LOAD ... FROM FILE(S) X
    auto& i = q->getImportSources();

    EXPECT_EQ(importSources.size(), i.size());

    if (importSources.size() == i.size()) {
      for (size_t j = 0; j < i.size(); ++j) {
        equalImportSource(importSources[j], i[j]);
      }
    }

    // STORE AS FILE
    if (exportDestination == nullptr) {
      EXPECT_TRUE(q->getExportDestination() == nullptr);
    } else {
      EXPECT_TRUE(q->getExportDestination() != nullptr);
      EXPECT_STREQ(exportDestination->toString().c_str(),
                   q->getExportDestination()->toString().c_str());
    }

    // DELETE
    EXPECT_STREQ(delete_var.c_str(), q->getDelete().c_str());

    // CHOOSE
    checkEqual(q->getPredicate(), choosePred);

    // AS (normal)
    EXPECT_EQ(proj.size(), q->getProjectors().size());
    for (size_t i = 0; i < q->getProjectors().size(); ++i) {
      auto& info = typeid(*q->getProjectors()[i].get());
      auto& typeInfo = typeid(*proj[i].get());
      EXPECT_EQ(info, typeInfo);
      EXPECT_STREQ(q->getProjectors()[i]->toString().c_str(),
                   proj[i]->toString().c_str());
    }

    // AS (Flat)
    EXPECT_EQ(setproj.size(), q->getSetProjectors().size());
    for (size_t i = 0; i < q->getSetProjectors().size(); ++i) {
      EXPECT_STREQ(q->getSetProjectors()[i]->toString().c_str(),
                   setproj[i]->toString().c_str());
    }

    // Agg
    EXPECT_EQ(agg.size(), q->getAggregators().size());
    for (size_t i = 0; i < q->getAggregators().size(); ++i) {
      EXPECT_STREQ(q->getAggregators()[i]->getDestPointer().c_str(),
                   agg[i]->getDestPointer().c_str());
      EXPECT_STREQ(q->getAggregators()[i]->getName().c_str(),
                   agg[i]->getName().c_str());
    }
  }
};

TEST_F(QueryParsingTest, Analyze) {
  EXPECT_EQ(joda::queryparsing::QueryParser::checkLanguage(), 0);
}

/*
 * LOAD Tests
 */

TEST_F(QueryParsingTest, SimpleLoad) {
  std::shared_ptr<joda::query::Query> q;
  EXPECT_NO_THROW(q = parseQuery("LOAD A"););
  EXPECT_NE(q, nullptr);
  equals(q, "A", default_importSources, default_exportDestination,
         default_deleteVar, default_choosePred, default_proj, default_setproj,
         default_agg);
}

TEST_F(QueryParsingTest, LOADLoad) {
  std::shared_ptr<joda::query::Query> q;
  EXPECT_NO_THROW(q = parseQuery("LOAD LOAD"););
  EXPECT_NE(q, nullptr);
  equals(q, "LOAD", default_importSources, default_exportDestination,
         default_deleteVar, default_choosePred, default_proj, default_setproj,
         default_agg);
}

TEST_F(QueryParsingTest, LoadFile) {
  std::shared_ptr<joda::query::Query> q;
  EXPECT_NO_THROW(q = parseQuery("LOAD A FROM FILE \"json.json\""););
  EXPECT_NE(q, nullptr);
  std::vector<std::unique_ptr<joda::docparsing::IImportSource>> sources;
  sources.push_back(std::make_unique<joda::docparsing::JSONFileSource>(
      "json.json", false, 1));
  equals(q, "A", sources, default_exportDestination, default_deleteVar,
         default_choosePred, default_proj, default_setproj, default_agg);
}

TEST_F(QueryParsingTest, LoadFiles) {
  std::shared_ptr<joda::query::Query> q;
  EXPECT_NO_THROW(q = parseQuery("LOAD A FROM FILES \"tmp\""););
  EXPECT_NE(q, nullptr);
  std::vector<std::unique_ptr<joda::docparsing::IImportSource>> sources;
  sources.push_back(std::make_unique<joda::docparsing::JSONFileDirectorySource>(
      "tmp", false, 1));
  equals(q, "A", sources, default_exportDestination, default_deleteVar,
         default_choosePred, default_proj, default_setproj, default_agg);
}

TEST_F(QueryParsingTest, LoadFileNoVar) {
  std::shared_ptr<joda::query::Query> q;
  q = parseQuery("LOAD FROM FILE \"json.json\"", false);
  EXPECT_EQ(q, nullptr);
}

TEST_F(QueryParsingTest, LoadMissingVar) {
  std::shared_ptr<joda::query::Query> q;
  q = parseQuery("LOAD ", false);
  EXPECT_EQ(q, nullptr);
}

TEST_F(QueryParsingTest, MissingLoad) {
  std::shared_ptr<joda::query::Query> q;
  q = parseQuery("STORE B", false);
  EXPECT_EQ(q, nullptr);
}

TEST_F(QueryParsingTest, LoadFileSample) {
  std::shared_ptr<joda::query::Query> q;
  EXPECT_NO_THROW(q = parseQuery("LOAD A FROM FILE \"json.json\" SAMPLE 0.5"););
  EXPECT_NE(q, nullptr);
  std::vector<std::unique_ptr<joda::docparsing::IImportSource>> sources;
  sources.push_back(std::make_unique<joda::docparsing::JSONFileSource>(
      "json.json", false, 0.5));
  equals(q, "A", sources, default_exportDestination, default_deleteVar,
         default_choosePred, default_proj, default_setproj, default_agg);
}

TEST_F(QueryParsingTest, MultipleLoads) {
  std::shared_ptr<joda::query::Query> q;
  std::string query = R"#(
LOAD A
FROM FILE "json.json" SAMPLE 0.5  ,
FROM FILES "tmp" SAMPLE 0.2,
FROM FILE "one.json" LINESEPARATED,
FROM FILES "tmp2" LINESEPARATED
)#";
  EXPECT_NO_THROW(q = parseQuery(query););
  EXPECT_NE(q, nullptr);
  std::vector<std::unique_ptr<joda::docparsing::IImportSource>> sources;
  sources.push_back(std::make_unique<joda::docparsing::JSONFileSource>(
      "json.json", false, 0.5));
  sources.push_back(std::make_unique<joda::docparsing::JSONFileDirectorySource>(
      "tmp", false, 0.2));
  sources.push_back(
      std::make_unique<joda::docparsing::JSONFileSource>("one.json", true, 1));
  sources.push_back(std::make_unique<joda::docparsing::JSONFileDirectorySource>(
      "tmp2", true, 1));
  equals(q, "A", sources, default_exportDestination, default_deleteVar,
         default_choosePred, default_proj, default_setproj, default_agg);
}

TEST_F(QueryParsingTest, LoadFileSample2) {
  std::shared_ptr<joda::query::Query> q;
  EXPECT_NO_THROW(q = parseQuery("LOAD A FROM FILE \"json.json\" LINESEPARATED "
                                 "SAMPLE 0.382783464824757"););
  EXPECT_NE(q, nullptr);
  std::vector<std::unique_ptr<joda::docparsing::IImportSource>> sources;
  sources.push_back(std::make_unique<joda::docparsing::JSONFileSource>(
      "json.json", true, 0.382783464824757));
  equals(q, "A", sources, default_exportDestination, default_deleteVar,
         default_choosePred, default_proj, default_setproj, default_agg);
}

TEST_F(QueryParsingTest, LoadFileSampleNoNum) {
  std::shared_ptr<joda::query::Query> q;
  q = parseQuery("LOAD A FROM FILE \"json.json\" SAMPLE ", false);
  EXPECT_EQ(q, nullptr);
}

TEST_F(QueryParsingTest, LoadFileSampleOne) {
  std::shared_ptr<joda::query::Query> q;
  q = parseQuery("LOAD A FROM FILE \"json.json\" SAMPLE 1", false);
  EXPECT_EQ(q, nullptr);
}

TEST_F(QueryParsingTest, LoadFileSampleLargerOne) {
  std::shared_ptr<joda::query::Query> q;
  q = parseQuery("LOAD A FROM FILE \"json.json\" SAMPLE 1.5", false);
  EXPECT_EQ(q, nullptr);
}

TEST_F(QueryParsingTest, LoadFileSampleMinus) {
  std::shared_ptr<joda::query::Query> q;
  q = parseQuery("LOAD A FROM FILE \"json.json\" SAMPLE -0.5", false);
  EXPECT_EQ(q, nullptr);
}

/*
 * STORE Tests
 */
TEST_F(QueryParsingTest, SimpleStore) {
  std::shared_ptr<joda::query::Query> q;
  EXPECT_NO_THROW(q = parseQuery("LOAD A STORE B"););
  EXPECT_NE(q, nullptr);
  auto store = std::make_shared<JSONStorage>("B");
  std::unique_ptr<IExportDestination> e =
      std::make_unique<StorageExport>(store);
  equals(q, "A", default_importSources, e, default_deleteVar,
         default_choosePred, default_proj, default_setproj, default_agg);
}

TEST_F(QueryParsingTest, StoreFILE) {
  std::shared_ptr<joda::query::Query> q;
  EXPECT_NO_THROW(q = parseQuery("LOAD A STORE AS FILE \"json.json\""););
  EXPECT_NE(q, nullptr);
  std::unique_ptr<IExportDestination> e =
      std::make_unique<FileExport>("json.json");
  equals(q, "A", default_importSources, e, default_deleteVar,
         default_choosePred, default_proj, default_setproj, default_agg);
}

TEST_F(QueryParsingTest, StoreFileAndVar) {
  std::shared_ptr<joda::query::Query> q;
  q = parseQuery("LOAD A STORE B AS FILE \"json.json\"", false);
  EXPECT_EQ(q, nullptr);
}

/*
 * DELETE Tests
 */
TEST_F(QueryParsingTest, SimpleDelete) {
  std::shared_ptr<joda::query::Query> q;
  EXPECT_NO_THROW(q = parseQuery("LOAD A DELETE B"););
  EXPECT_NE(q, nullptr);
  equals(q, "A", default_importSources, default_exportDestination, "B",
         default_choosePred, default_proj, default_setproj, default_agg);
}

TEST_F(QueryParsingTest, DeleteMissingVar) {
  std::shared_ptr<joda::query::Query> q;
  q = parseQuery("LOAD A DELETE ", false);
  EXPECT_EQ(q, nullptr);
}

/*
 * CHOOSE
 */
TEST_F(QueryParsingTest, ChooseTrue) {
  std::shared_ptr<joda::query::Query> q;
  auto truePred = IValToPred(std::make_unique<joda::query::BoolProvider>(true));
  EXPECT_NO_THROW(q = parseQuery("LOAD A CHOOSE true"););
  EXPECT_NE(q, nullptr);
  equals(q, "A", default_importSources, default_exportDestination,
         default_deleteVar, truePred, default_proj, default_setproj,
         default_agg);
}

TEST_F(QueryParsingTest, ChooseFalse) {
  std::shared_ptr<joda::query::Query> q;
  auto falsePred =
      IValToPred(std::make_unique<joda::query::BoolProvider>(false));
  EXPECT_NO_THROW(q = parseQuery("LOAD A CHOOSE false"););
  EXPECT_NE(q, nullptr);
  equals(q, "A", default_importSources, default_exportDestination,
         default_deleteVar, falsePred, default_proj, default_setproj,
         default_agg);
}

TEST_F(QueryParsingTest, ChooseNumber) {
  std::shared_ptr<joda::query::Query> q;
  q = parseQuery("LOAD A CHOOSE 1 ", false);
  EXPECT_EQ(q, nullptr);
}

TEST_F(QueryParsingTest, StaticEvalAnd) {
  std::shared_ptr<joda::query::Query> q;
  auto falsePred =
      IValToPred(std::make_unique<joda::query::BoolProvider>(false));
  EXPECT_NO_THROW(q = parseQuery("LOAD A CHOOSE true && false"););
  EXPECT_NE(q, nullptr);
  equals(q, "A", default_importSources, default_exportDestination,
         default_deleteVar, falsePred, default_proj, default_setproj,
         default_agg);
}

TEST_F(QueryParsingTest, StaticEvalOr) {
  std::shared_ptr<joda::query::Query> q;
  auto truePred = IValToPred(std::make_unique<joda::query::BoolProvider>(true));
  EXPECT_NO_THROW(q = parseQuery("LOAD A CHOOSE true || false"););
  EXPECT_NE(q, nullptr);
  equals(q, "A", default_importSources, default_exportDestination,
         default_deleteVar, truePred, default_proj, default_setproj,
         default_agg);
}

TEST_F(QueryParsingTest, ASValue) {
  std::shared_ptr<joda::query::Query> q;
  std::vector<std::unique_ptr<joda::query::IProjector>> proj;
  proj.push_back(std::make_unique<joda::query::ValueProviderProjector>(
      "/a", std::make_unique<joda::query::PiProvider>()));
  EXPECT_NO_THROW(q = parseQuery("LOAD A AS ('/a' : PI())"););
  EXPECT_NE(q, nullptr);
  equals(q, "A", default_importSources, default_exportDestination,
         default_deleteVar, default_choosePred, proj, default_setproj,
         default_agg);
}

TEST_F(QueryParsingTest, ASPtr) {
  std::shared_ptr<joda::query::Query> q;
  std::vector<std::unique_ptr<joda::query::IProjector>> proj;
  proj.push_back(std::make_unique<joda::query::ValueProviderProjector>(
      "/a", std::make_unique<joda::query::PointerProvider>("/b")));
  EXPECT_NO_THROW(q = parseQuery("LOAD A AS ('/a' : '/b')"););
  EXPECT_NE(q, nullptr);
  equals(q, "A", default_importSources, default_exportDestination,
         default_deleteVar, default_choosePred, proj, default_setproj,
         default_agg);
}

TEST_F(QueryParsingTest, ASAll) {
  std::shared_ptr<joda::query::Query> q;

  EXPECT_NO_THROW(q = parseQuery("LOAD A AS *"););
  EXPECT_NE(q, nullptr);
  equals(q, "A", default_importSources, default_exportDestination,
         default_deleteVar, default_choosePred, default_proj, default_setproj,
         default_agg);
}

TEST_F(QueryParsingTest, ASAllButOne) {
  std::shared_ptr<joda::query::Query> q;
  std::vector<std::unique_ptr<joda::query::IProjector>> proj;
  proj.push_back(std::make_unique<joda::query::PointerCopyProject>("", ""));
  proj.push_back(std::make_unique<joda::query::DeletePointerProjector>("/a"));
  EXPECT_NO_THROW(q = parseQuery("LOAD A AS *, ('/a':)"););
  EXPECT_NE(q, nullptr);
  equals(q, "A", default_importSources, default_exportDestination,
         default_deleteVar, default_choosePred, proj, default_setproj,
         default_agg);
}

TEST_F(QueryParsingTest, ASMultiple) {
  std::shared_ptr<joda::query::Query> q;
  std::vector<std::unique_ptr<joda::query::IProjector>> proj;
  proj.push_back(std::make_unique<joda::query::PointerCopyProject>("", ""));
  proj.push_back(std::make_unique<joda::query::DeletePointerProjector>("/a"));
  proj.push_back(std::make_unique<joda::query::ValueProviderProjector>(
      "/b", std::make_unique<joda::query::PiProvider>()));
  EXPECT_NO_THROW(q = parseQuery("LOAD A AS *, ('/a':),   ('/b' : PI())"););
  EXPECT_NE(q, nullptr);
  equals(q, "A", default_importSources, default_exportDestination,
         default_deleteVar, default_choosePred, proj, default_setproj,
         default_agg);
}

TEST_F(QueryParsingTest, MissingASExp) {
  std::shared_ptr<joda::query::Query> q;
  q = parseQuery("LOAD A AS ", false);
  EXPECT_EQ(q, nullptr);
}

TEST_F(QueryParsingTest, QueryToStringTest) {
  std::shared_ptr<joda::query::Query> q;
  std::string qstring =
      R"(LOAD A FROM FILES "/dir" CHOOSE '' == 5.000000 AS ('/old':'') AGG ('':SUM('/old')))";
  EXPECT_NO_THROW(q = parseQuery(qstring));
  EXPECT_STREQ(qstring.c_str(), q->toString().c_str());
}

TEST_F(QueryParsingTest, NumParseTest) {
  std::shared_ptr<joda::query::Query> q;
  std::string qstring =
      R"(LOAD A CHOOSE ('' == 5.000000 || ('' == 3 || ('' == -4 || '' == -4.789000))))";
  EXPECT_NO_THROW(q = parseQuery(qstring));
  EXPECT_STREQ(qstring.c_str(), q->toString().c_str());
}

TEST_F(QueryParsingTest, MixedAggTest) {
  std::shared_ptr<joda::query::Query> q;
  std::string qstring =
      R"(LOAD A AGG ('/sum':SUM('/old')), ('/count':GROUP COUNT('/old') BY EXISTS('')), ('/dist':DISTINCT('/old')), ('/collect':GROUP COLLECT('/old') AS coll BY '')  )";
  EXPECT_NO_THROW(q = parseQuery(qstring));

  ASSERT_NE(q, nullptr);
  auto& aggs = q->getAggregators();
  ASSERT_EQ(aggs.size(), 4);

  EXPECT_STREQ(aggs[0]->toString().c_str(), "'/sum':SUM('/old')");
  EXPECT_STREQ(aggs[1]->toString().c_str(),
               "'/count':GROUP COUNT('/old') BY EXISTS('')");
  EXPECT_STREQ(aggs[2]->toString().c_str(), "'/dist':DISTINCT('/old')");
  EXPECT_STREQ(aggs[3]->toString().c_str(),
               "'/collect':GROUP COLLECT('/old') AS coll BY ''");
}