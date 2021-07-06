//
// Created by Nico on 15/05/2019.
//

#include <gtest/gtest.h>
#include <joda/misc/Timer.h>
#include <rapidjson/reader.h>
#include "../src/RJPathsReader.h"

/*
 * RJPath
 */
class RJPathTest : public ::testing::Test {
 public:
  std::vector<std::string> getPaths(std::string& doc) {
    RJPathsReader handler;
    rapidjson::Reader reader;
    rapidjson::StringStream stream(doc.c_str());
    reader.Parse(stream, handler);
    return handler.getPaths();
  }

 protected:
  std::vector<std::string> testJSON{
      R"!({"menu":{"id":"file","value":"File","popup":{"menuitem":[{"value":"New","onclick":"CreateNewDoc()"},{"value":"Open","onclick":"OpenDoc()"},{"value":"Close","onclick":"CloseDoc()"}]}}})!",
      R"!({"header":"SVG Viewer","items":[1,"string",{"id":"Open"},true,{"id":"OpenNew","label":"Open New"}],"test":1,"nested":{"items":[1,"string",{"id":"Open"},true,{"id":"OpenNew","label":"Open New"}]},"unnested":true})!",
      R"!([[[],[]],[[]],[]])!",
      R"!([[[],1,2.5,[{"key":"value"}],null,-10,"string"],[[]],[{"key":"value"}]])!"};
};

TEST_F(RJPathTest, RJPathTest1) {
  auto paths = getPaths(testJSON[0]);
  std::vector<std::string> controlPaths = {"/menu",
                                           "/menu/id",
                                           "/menu/value",
                                           "/menu/popup",
                                           "/menu/popup/menuitem",
                                           "/menu/popup/menuitem/0/value",
                                           "/menu/popup/menuitem/0/onclick",
                                           "/menu/popup/menuitem/1/value",
                                           "/menu/popup/menuitem/1/onclick",
                                           "/menu/popup/menuitem/2/value",
                                           "/menu/popup/menuitem/2/onclick"};
  EXPECT_EQ(paths.size(), controlPaths.size());
  std::sort(paths.begin(), paths.end());
  std::sort(controlPaths.begin(), controlPaths.end());
  EXPECT_TRUE(std::equal(paths.begin(), paths.end(), controlPaths.begin()));
}

TEST_F(RJPathTest, RJPathTest2) {
  auto paths = getPaths(testJSON[1]);
  std::vector<std::string> controlPaths = {"/header",
                                           "/items",
                                           "/items/2/id",
                                           "/items/4/id",
                                           "/items/4/label",
                                           "/test",
                                           "/nested",
                                           "/nested/items",
                                           "/nested/items/2/id",
                                           "/nested/items/4/id",
                                           "/nested/items/4/label",
                                           "/unnested"};

  EXPECT_EQ(paths.size(), controlPaths.size());
  std::sort(paths.begin(), paths.end());
  std::sort(controlPaths.begin(), controlPaths.end());
  EXPECT_TRUE(std::equal(paths.begin(), paths.end(), controlPaths.begin()));
}

TEST_F(RJPathTest, RJPathTest3) {
  auto paths = getPaths(testJSON[2]);
  std::vector<std::string> controlPaths = {};

  EXPECT_EQ(paths.size(), controlPaths.size());
  std::sort(paths.begin(), paths.end());
  std::sort(controlPaths.begin(), controlPaths.end());
  EXPECT_TRUE(std::equal(paths.begin(), paths.end(), controlPaths.begin()));
}

TEST_F(RJPathTest, RJPathTest4) {
  auto paths = getPaths(testJSON[3]);
  std::vector<std::string> controlPaths = {"/0/3/0/key", "/2/0/key"};

  EXPECT_EQ(paths.size(), controlPaths.size());
  std::sort(paths.begin(), paths.end());
  std::sort(controlPaths.begin(), controlPaths.end());
  EXPECT_TRUE(std::equal(paths.begin(), paths.end(), controlPaths.begin()));
}