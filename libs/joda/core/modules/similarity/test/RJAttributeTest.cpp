//
// Created by Nico on 15/05/2019.
//

#include <gtest/gtest.h>
#include <rapidjson/reader.h>
#include <joda/misc/Timer.h>
#include "../src/RJAttributeReader.h"
#include <set>

/*
 * RJPath
 */
class RJAttributeTest : public ::testing::Test {
 public:
  std::set<std::string> getAttributes(std::string &doc) {
    RJAttributeReader handler;
    rapidjson::Reader reader;
    rapidjson::StringStream stream(doc.c_str());
    reader.Parse(stream, handler);
    return handler.getAttributes();
  }

 protected:

  std::vector<std::string> testJSON{
      R"!({"menu":{"id":"file","value":"File","popup":{"menuitem":[{"value":"New","onclick":"CreateNewDoc()"},{"value":"Open","onclick":"OpenDoc()"},{"value":"Close","onclick":"CloseDoc()"}]}}})!",
      R"!({"header":"SVG Viewer","items":[1,"string",{"id":"Open"},true,{"id":"OpenNew","label":"Open New"}],"test":1,"nested":{"items":[1,"string",{"id":"Open"},true,{"id":"OpenNew","label":"Open New"}]},"unnested":true})!",
      R"!([[[],[]],[[]],[]])!",
      R"!([[[],1,2.5,[{"key":"value"}],null,-10,"string"],[[]],[{"key":"value"}]])!"
  };
};

TEST_F(RJAttributeTest, RJAttributeTest1) {
  auto paths = getAttributes(testJSON[0]);
  std::set<std::string> controlPaths = {
      "menu",
      "id",
      "value",
      "popup",
      "menuitem",
      "onclick",
  };
  EXPECT_EQ(paths.size(), controlPaths.size());
  EXPECT_TRUE(std::equal(paths.begin(), paths.end(), controlPaths.begin()));

}

TEST_F(RJAttributeTest, RJAttributeTest2) {
  auto paths = getAttributes(testJSON[1]);
  std::set<std::string> controlPaths = {
      "header",
      "items",
      "id",
      "label",
      "test",
      "nested",
      "unnested"
  };

  EXPECT_EQ(paths.size(), controlPaths.size());
  EXPECT_TRUE(std::equal(paths.begin(), paths.end(), controlPaths.begin()));

}

TEST_F(RJAttributeTest, RJAttributeTest3) {
  auto paths = getAttributes(testJSON[2]);
  std::set<std::string> controlPaths = {};

  EXPECT_EQ(paths.size(), controlPaths.size());
  EXPECT_TRUE(std::equal(paths.begin(), paths.end(), controlPaths.begin()));

}

TEST_F(RJAttributeTest, RJAttributeTest4) {
  auto paths = getAttributes(testJSON[3]);
  std::set<std::string> controlPaths = {
      "key",
  };

  EXPECT_EQ(paths.size(), controlPaths.size());
  EXPECT_TRUE(std::equal(paths.begin(), paths.end(), controlPaths.begin()));

}