//
// Created by Nico on 28/02/2019.
//

#include <gtest/gtest.h>
#include <joda/queryparsing/QueryParser.h>
#include <joda/queryexecution/QueryPlan.h>
#include <boost/program_options/variables_map.hpp>
#include <joda/config/ConfigParser.h>
#include <joda/fs/DirectoryRegister.h>
#include <joda/storage/collection/StorageCollection.h>

class FullStack : public ::testing::Test {
 public:
  FullStack() : numtestjsons() {
    char array[] = "joda";
    char *a  =  &array[0];
    char **argv = &a;
    FLAGS_minloglevel = 0;
    joda::filesystem::DirectoryRegister::getInstance();
    auto options =  ConfigParser::parseConfigs(1, argv);
    ConfigParser::setConfig(options);
#ifdef JODA_TEST_NOSTORE_MODE
    config::storeJson = false;
#endif
    initNumtest();

    StorageCollection::getInstance().removeStorage("A");

  }

  static unsigned long executeQuery(const char* qstr) {

      joda::queryparsing::QueryParser qp;
      auto q =  qp.parse(qstr);
    EXPECT_NE(q, nullptr) << qp.getLastError();
    if (q == nullptr) return 0;
      QueryPlan plan(q);
        return plan.executeQuery(nullptr);

  }

 protected:
  void initNumtest(){
    numtestjsons.emplace_back(R"({"desc":"ones","a":1,"b":1,"c":1})");
    numtestjsons.emplace_back(R"({"desc":"asc","a":1,"b":2,"c":3})");
    numtestjsons.emplace_back(R"({"desc":"zerofront","a":0,"b":1,"c":1})");
    numtestjsons.emplace_back(R"({"desc":"zeroback","a":1,"b":1,"c":0})");
    numtestjsons.emplace_back(R"({"desc":"missing_b","a":1,"c":1})");
  }
  std::vector<std::string> numtestjsons;

};

TEST_F(FullStack, LOADNumtest) {
  auto resultId = executeQuery("LOAD A FROM FILE \"./test/data/json/numtest.json\" LINESEPARATED");
  auto result = StorageCollection::getInstance().getStorage(resultId);
  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 5);
  auto stringify = result->stringify(0, 4);
  for (const auto &item : stringify) {
    EXPECT_TRUE(std::find(numtestjsons.begin(), numtestjsons.end(), item) != numtestjsons.end());
  }

}

TEST_F(FullStack, LOADNumtestSimJaccard) {
  config::sim_measure = config::PATH_JACCARD;
  auto resultId = executeQuery("LOAD PATH_JACCARD FROM FILE \"./test/data/json/numtest.json\" LINESEPARATED ");
  auto result = StorageCollection::getInstance().getStorage(resultId);
  EXPECT_NE(result,nullptr);
  ASSERT_EQ(result->size(), 5);
  auto stringify = result->stringify(0, 4);
  for (const auto &item : stringify) {
    EXPECT_TRUE(std::find(numtestjsons.begin(), numtestjsons.end(), item) != numtestjsons.end());
  }
  config::sim_measure = config::NO_SIMILARITY;
}

TEST_F(FullStack, LOADFormattest) {
  joda::queryparsing::QueryParser qp;
  auto q = qp.parse("LOAD B FROM FILE \"./test/data/json/formattest.json\"");
  ASSERT_NE(q, nullptr) << qp.getLastError();
  QueryPlan plan(q);
  auto resultId = plan.executeQuery(nullptr);
  auto result = StorageCollection::getInstance().getStorage(resultId);
  EXPECT_NE(result,nullptr);
  EXPECT_EQ(result->size(),5);
  auto stringify = result->stringify(0, 4);
  EXPECT_EQ(result->size(),stringify.size());
  for (const auto &item : stringify) {
    EXPECT_TRUE(std::find(numtestjsons.begin(), numtestjsons.end(), item) != numtestjsons.end());
  }
}

TEST_F(FullStack, CHOOSEString) {
  ASSERT_FALSE(StorageCollection::getInstance().storageExists("A"));
  executeQuery("LOAD A FROM FILE \"./test/data/json/numtest.json\" LINESEPARATED");
  ASSERT_TRUE(StorageCollection::getInstance().storageExists("A"));
  ASSERT_EQ(StorageCollection::getInstance().getStorage("A")->size(), 5);
  joda::queryparsing::QueryParser qp;
  auto q =  qp.parse("LOAD A CHOOSE '/desc' == \"ones\"");
  ASSERT_NE(q, nullptr) << qp.getLastError();
  QueryPlan plan(q);
  auto resultId = plan.executeQuery(nullptr);
  auto result = StorageCollection::getInstance().getStorage(resultId);
  EXPECT_NE(result,nullptr);
  ASSERT_EQ(result->size(), 1);
  auto stringify = result->stringify(0, 0);
  EXPECT_STREQ(stringify[0].c_str(),numtestjsons[0].c_str());
}

TEST_F(FullStack, CHOOSENum) {
  ASSERT_FALSE(StorageCollection::getInstance().storageExists("A"));
  executeQuery("LOAD A FROM FILE \"./test/data/json/numtest.json\" LINESEPARATED");
  ASSERT_TRUE(StorageCollection::getInstance().storageExists("A"));
  ASSERT_EQ(StorageCollection::getInstance().getStorage("A")->size(), 5);
  joda::queryparsing::QueryParser qp;
  auto q =  qp.parse("LOAD A CHOOSE '/c' == 1");
  ASSERT_NE(q, nullptr) << qp.getLastError();
  QueryPlan plan(q);
  auto resultId = plan.executeQuery(nullptr);
  auto result = StorageCollection::getInstance().getStorage(resultId);
  EXPECT_NE(result,nullptr);
  EXPECT_EQ(result->size(),3);
}

TEST_F(FullStack, ChooseConstTrue) {
  ASSERT_FALSE(StorageCollection::getInstance().storageExists("A"));
  executeQuery("LOAD A FROM FILE \"./test/data/json/numtest.json\" LINESEPARATED");
  ASSERT_TRUE(StorageCollection::getInstance().storageExists("A"));
  ASSERT_EQ(StorageCollection::getInstance().getStorage("A")->size(), 5);
  joda::queryparsing::QueryParser qp;
  auto q = qp.parse("LOAD A CHOOSE 1 == 1");
  ASSERT_NE(q, nullptr) << qp.getLastError();
  QueryPlan plan(q);
  auto resultId = plan.executeQuery(nullptr);
  auto result = StorageCollection::getInstance().getStorage(resultId);
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(result->size(), 5);
}

TEST_F(FullStack, ChooseConstFalse) {
  ASSERT_FALSE(StorageCollection::getInstance().storageExists("A"));
  executeQuery("LOAD A FROM FILE \"./test/data/json/numtest.json\" LINESEPARATED");
  ASSERT_TRUE(StorageCollection::getInstance().storageExists("A"));
  ASSERT_EQ(StorageCollection::getInstance().getStorage("A")->size(), 5);
  joda::queryparsing::QueryParser qp;
  auto q = qp.parse("LOAD A CHOOSE 1 != 1");
  ASSERT_NE(q, nullptr) << qp.getLastError();
  QueryPlan plan(q);
  auto resultId = plan.executeQuery(nullptr);
  auto result = StorageCollection::getInstance().getStorage(resultId);
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(result->size(), 0);
}

TEST_F(FullStack, AGGNum) {
  ASSERT_FALSE(StorageCollection::getInstance().storageExists("A"));
  executeQuery("LOAD A FROM FILE \"./test/data/json/numtest.json\" LINESEPARATED");
  ASSERT_TRUE(StorageCollection::getInstance().storageExists("A"));
  ASSERT_EQ(StorageCollection::getInstance().getStorage("A")->size(), 5);
  joda::queryparsing::QueryParser qp;
  auto q =  qp.parse("LOAD A AGG ('':SUM('/b'))");
  ASSERT_NE(q, nullptr) << qp.getLastError();
  QueryPlan plan(q);
  auto resultId = plan.executeQuery(nullptr);
  auto result = StorageCollection::getInstance().getStorage(resultId);
  EXPECT_NE(result,nullptr);
  ASSERT_EQ(result->size(), 1);
  auto stringify = result->stringify(0, 0);
  EXPECT_STREQ(stringify[0].c_str(),"5.0");
}

TEST_F(FullStack, GroupAggNum) {
  ASSERT_FALSE(StorageCollection::getInstance().storageExists("A"));
  executeQuery("LOAD A FROM FILE \"./test/data/json/numtest.json\" LINESEPARATED");
  ASSERT_TRUE(StorageCollection::getInstance().storageExists("A"));
  ASSERT_EQ(StorageCollection::getInstance().getStorage("A")->size(), 5);
  joda::queryparsing::QueryParser qp;
  auto q = qp.parse("LOAD A AGG ('': GROUP COUNT('/a') AS count BY '/c' )");
  ASSERT_NE(q, nullptr) << qp.getLastError();
  QueryPlan plan(q);
  auto resultId = plan.executeQuery(nullptr);
  auto result = StorageCollection::getInstance().getStorage(resultId);
  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  auto stringify = result->stringify(0, 0);
  auto docs = result->getRaw(0, 0);
  auto &doc = docs[0];
  ASSERT_TRUE(doc->IsArray());
  ASSERT_EQ(doc->Size(), 3);
  for (const auto &item : doc->GetArray()) {
    ASSERT_TRUE(item.IsObject());
    ASSERT_TRUE(item.HasMember("count"));
    ASSERT_TRUE(item.HasMember("group"));
    const auto &count = *item.FindMember("count");
    const auto &group = *item.FindMember("group");
    ASSERT_TRUE(group.value.IsNumber());
    ASSERT_TRUE(count.value.IsNumber());
    switch (static_cast<int>(group.value.GetDouble())) {
      case 0:EXPECT_EQ(count.value.GetDouble(), 1);
        break;
      case 1:EXPECT_EQ(count.value.GetDouble(), 3);
        break;
      case 3:EXPECT_EQ(count.value.GetDouble(), 1);
        break;
      default:FAIL() << "Wrong Group";
    }

  }
}

TEST_F(FullStack, Proj) {
  ASSERT_FALSE(StorageCollection::getInstance().storageExists("A"));
  executeQuery("LOAD A FROM FILE \"./test/data/json/numtest.json\" LINESEPARATED");
  ASSERT_TRUE(StorageCollection::getInstance().storageExists("A"));
  ASSERT_EQ(StorageCollection::getInstance().getStorage("A")->size(), 5);
  joda::queryparsing::QueryParser qp;
  auto q =  qp.parse("LOAD A AS ('/a': \"fixed\")");
  ASSERT_NE(q, nullptr) << qp.getLastError();
  QueryPlan plan(q);
  auto resultId = plan.executeQuery(nullptr);
  auto result = StorageCollection::getInstance().getStorage(resultId);
  EXPECT_NE(result,nullptr);
  ASSERT_EQ(result->size(), 5);
  auto stringify = result->stringify(0, 4);
  for (const auto &item : stringify) {
    EXPECT_STREQ(item.c_str(), R"({"a":"fixed"})");
  }

}

TEST_F(FullStack, Delete) {
  ASSERT_FALSE(StorageCollection::getInstance().storageExists("A"));
  executeQuery("LOAD A FROM FILE \"./test/data/json/numtest.json\" LINESEPARATED");
  ASSERT_TRUE(StorageCollection::getInstance().storageExists("A"));
  ASSERT_EQ(StorageCollection::getInstance().getStorage("A")->size(), 5);
  joda::queryparsing::QueryParser qp;
  auto q =  qp.parse("LOAD A DELETE A");
  ASSERT_NE(q, nullptr) << qp.getLastError();
  QueryPlan plan(q);
  EXPECT_TRUE(StorageCollection::getInstance().storageExists("A"));
  auto resultId = plan.executeQuery(nullptr);
  EXPECT_FALSE(StorageCollection::getInstance().storageExists("A"));

}