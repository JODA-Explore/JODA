//
// Created by Nico on 15/05/2019.
//


#include <gtest/gtest.h>
#include <joda/misc/RJFwd.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <joda/document/view/ViewLayer.h>

/*
 * ViewTest
 */
class ViewTest : public ::testing::Test {
 public:

 protected:
 protected:
  virtual void SetUp() {
    baseDoc = parseDoc(baseDocStr);
  }

  static RJDocument parseDoc(const std::string &str) {
    RJDocument doc;
    doc.Parse(str);
    if (doc.HasParseError()) throw (std::runtime_error("Document has parse error"));
    return doc;
  }

  static std::string stringify(const RJDocument &doc) {
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);
    doc.Accept(writer);
    return buff.GetString();
  }

  static std::string stringify(ViewLayer &layer) {
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);
    layer.Accept(writer);
    return buff.GetString();
  }

  std::string baseDocStr = R"({"A":{"B":1,"C":2},"D":2})";
  std::vector<std::string> baseDocPaths{""};
  RJDocument baseDoc;
  ViewStructure struc;
};

TEST_F(ViewTest, BaseLayerTest) {
  auto base = std::make_unique<ViewLayer>(&baseDoc, &baseDocPaths, nullptr, &struc);
  auto baseAccepted = stringify(*base);

  EXPECT_STRCASEEQ(baseDocStr.c_str(), baseAccepted.c_str());
}





/*
 * Single Layer - Top Level
 */



TEST_F(ViewTest, SingleViewDeleteTop) {
  auto base = std::make_unique<ViewLayer>(&baseDoc, &baseDocPaths, nullptr, &struc);

  std::string derivedStr = R"({})";
  std::vector<std::string> derivedPaths{"/D"};
  auto derivedDoc = parseDoc(derivedStr);
  ViewLayer derived(&derivedDoc, &derivedPaths, base.get(), &struc);

  auto derivedAccepted = stringify(derived);

  EXPECT_STRCASEEQ(R"({"A":{"B":1,"C":2}})", derivedAccepted.c_str());
}

TEST_F(ViewTest, SingleViewChangeTop) {
  auto base = std::make_unique<ViewLayer>(&baseDoc, &baseDocPaths, nullptr, &struc);

  std::string derivedStr = R"({"A":1})";
  std::vector<std::string> derivedPaths{"/A"};
  auto derivedDoc = parseDoc(derivedStr);
  ViewLayer derived(&derivedDoc, &derivedPaths, base.get(), &struc);

  auto derivedAccepted = stringify(derived);

  EXPECT_STRCASEEQ(R"({"A":1,"D":2})", derivedAccepted.c_str());
}

TEST_F(ViewTest, SingleViewAddTop) {
  auto base = std::make_unique<ViewLayer>(&baseDoc, &baseDocPaths, nullptr, &struc);

  std::string derivedStr = R"({"E":"Y"})";
  std::vector<std::string> derivedPaths{"/E"};
  auto derivedDoc = parseDoc(derivedStr);
  ViewLayer derived(&derivedDoc, &derivedPaths, base.get(), &struc);

  auto derivedAccepted = stringify(derived);

  EXPECT_STRCASEEQ(R"({"A":{"B":1,"C":2},"D":2,"E":"Y"})", derivedAccepted.c_str());
}


/*
 * Single Layer - Nested Level
 */

TEST_F(ViewTest, SingleViewDeleteNested) {
  auto base = std::make_unique<ViewLayer>(&baseDoc, &baseDocPaths, nullptr, &struc);

  std::string derivedStr = R"({"A":{}})";
  std::vector<std::string> derivedPaths{"/A/B"};
  auto derivedDoc = parseDoc(derivedStr);
  ViewLayer derived(&derivedDoc, &derivedPaths, base.get(), &struc);

  auto derivedAccepted = stringify(derived);

  EXPECT_STRCASEEQ(R"({"A":{"C":2},"D":2})", derivedAccepted.c_str());
}

TEST_F(ViewTest, SingleViewChangeNested) {
  auto base = std::make_unique<ViewLayer>(&baseDoc, &baseDocPaths, nullptr, &struc);

  std::string derivedStr = R"({"A":{"B":"X"}})";
  std::vector<std::string> derivedPaths{"/A/B"};
  auto derivedDoc = parseDoc(derivedStr);
  ViewLayer derived(&derivedDoc, &derivedPaths, base.get(), &struc);

  auto derivedAccepted = stringify(derived);

  EXPECT_STRCASEEQ(R"({"A":{"B":"X","C":2},"D":2})", derivedAccepted.c_str());
}

TEST_F(ViewTest, SingleViewAddNested) {
  auto base = std::make_unique<ViewLayer>(&baseDoc, &baseDocPaths, nullptr, &struc);

  std::string derivedStr = R"({"A":{"E":"Y"}})";
  std::vector<std::string> derivedPaths{"/A/E"};
  auto derivedDoc = parseDoc(derivedStr);
  ViewLayer derived(&derivedDoc, &derivedPaths, base.get(), &struc);

  auto derivedAccepted = stringify(derived);

  EXPECT_STRCASEEQ(R"({"A":{"B":1,"C":2,"E":"Y"},"D":2})", derivedAccepted.c_str());
}

TEST_F(ViewTest, SingleViewAddNewNested) {
  auto base = std::make_unique<ViewLayer>(&baseDoc, &baseDocPaths, nullptr, &struc);

  std::string derivedStr = R"({"E":{"F":"G"}})";
  std::vector<std::string> derivedPaths{"/E/F"};
  auto derivedDoc = parseDoc(derivedStr);
  ViewLayer derived(&derivedDoc, &derivedPaths, base.get(), &struc);

  auto derivedAccepted = stringify(derived);

  EXPECT_STRCASEEQ(R"({"A":{"B":1,"C":2},"D":2,"E":{"F":"G"}})", derivedAccepted.c_str());
}

TEST_F(ViewTest, SingleViewChangeParent) {
  auto base = std::make_unique<ViewLayer>(&baseDoc, &baseDocPaths, nullptr, &struc);

  std::string derivedStr = R"({"A":1})";
  std::vector<std::string> derivedPaths{"/A"};
  auto derivedDoc = parseDoc(derivedStr);
  ViewLayer derived(&derivedDoc, &derivedPaths, base.get(), &struc);

  auto derivedAccepted = stringify(derived);

  EXPECT_STRCASEEQ(R"({"A":1,"D":2})", derivedAccepted.c_str());
}
/*
 * Multi Layer
 */

TEST_F(ViewTest, DualViewMixed) {
  auto base = std::make_unique<ViewLayer>(&baseDoc, &baseDocPaths, nullptr, &struc);

  std::string firstStr = R"({"A":{"B":"X","E":5}})";
  std::vector<std::string> firstPaths{"/A/B", "/A/E", "/D"};
  auto firstDoc = parseDoc(firstStr);
  auto first = std::make_unique<ViewLayer>(&firstDoc, &firstPaths, base.get(), &struc);
  auto firstAccepted = stringify(*first);
  EXPECT_STRCASEEQ(R"({"A":{"B":"X","C":2,"E":5}})", firstAccepted.c_str());

  std::string secondStr = R"({"F":6,"D":{"X":10,"Y":20}})";
  std::vector<std::string> secondPaths{"/F", "/D"};
  auto secondDoc = parseDoc(secondStr);
  auto second = std::make_unique<ViewLayer>(&secondDoc, &secondPaths, first.get(), &struc);

  auto secondAccepted = stringify(*second);
  EXPECT_STRCASEEQ(R"({"A":{"B":"X","C":2,"E":5},"F":6,"D":{"X":10,"Y":20}})", secondAccepted.c_str());

}

TEST_F(ViewTest, DualChangeType) {
  auto base = std::make_unique<ViewLayer>(&baseDoc, &baseDocPaths, nullptr, &struc);

  std::string firstStr = R"({"E":1})";
  std::vector<std::string> firstPaths{"/E"};
  auto firstDoc = parseDoc(firstStr);
  auto first = std::make_unique<ViewLayer>(&firstDoc, &firstPaths, base.get(), &struc);
  auto firstAccepted = stringify(*first);
  EXPECT_STRCASEEQ(R"({"A":{"B":1,"C":2},"D":2,"E":1})", firstAccepted.c_str());

  std::string secondStr = R"({"E":{"F":"G"}})";
  std::vector<std::string> secondPaths{"/E/F"};
  auto secondDoc = parseDoc(secondStr);
  auto second = std::make_unique<ViewLayer>(&secondDoc, &secondPaths, first.get(), &struc);

  auto secondAccepted = stringify(*second);
  EXPECT_STRCASEEQ(R"({"A":{"B":1,"C":2},"D":2,"E":{"F":"G"}})", secondAccepted.c_str());

}

