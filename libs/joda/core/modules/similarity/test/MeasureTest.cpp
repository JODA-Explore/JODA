//
// Created by Nico on 15/05/2019.
//

#include <gtest/gtest.h>

#include <joda/misc/RJFwd.h>
#include <joda/similarity/measures/AttributeJaccard.h>
#include <joda/similarity/measures/PathJaccard.h>

template <typename T>
class MeasureTest : public testing::Test {
 public:
  typedef similarityRepresentation<T> SimRep;
  SimRep rep;
  T measure;
  typename SimRep::Representation getRepFromString(const std::string& str) {
    return rep.getRepresentation(str);
  }

  typename SimRep::Representation getRepFromDoc(const std::string& str) {
    RJDocument doc;
    doc.Parse(str.c_str());
    return rep.getRepresentation(doc);
  }

  RJDocument getDoc(const std::string& docstr) {
    RJDocument doc;
    doc.Parse(docstr.c_str());
    return doc;
  }

 protected:
  std::vector<std::string> testJSON{
      R"!({"menu":{"id":"file","value":"File","popup":{"menuitem":[{"value":"New","onclick":"CreateNewDoc()"},{"value":"Open","onclick":"OpenDoc()"},{"value":"Close","onclick":"CloseDoc()"}]}}})!",
      R"!({"header":"SVG Viewer","items":[1,"string",{"id":"Open"},true,{"id":"OpenNew","label":"Open New"}],"test":1,"nested":{"items":[1,"string",{"id":"Open"},true,{"id":"OpenNew","label":"Open New"}]},"unnested":true})!",
      R"!([[[],[]],[[]],[]])!",
      R"!([[[],1,[{"key":"value"}],"string"],[[]],[{"key":"value"}]])!"};
};

using SimilarityMeasures = testing::Types<PathJaccard, AttributeJaccard>;
TYPED_TEST_CASE(MeasureTest, SimilarityMeasures);

TYPED_TEST(MeasureTest, IsImplemented) {
  EXPECT_TRUE(this->rep.is_implemented);
}

TYPED_TEST(MeasureTest, StringDocEquality) {
  for (const auto& json : this->testJSON) {
    auto fromString = this->getRepFromString(json);
    auto fromDoc = this->getRepFromDoc(json);
    EXPECT_EQ(fromString, fromDoc);
  }
}

TYPED_TEST(MeasureTest, SelfSimilarity) {
  for (const auto& json : this->testJSON) {
    auto fromString = this->getRepFromString(json);
    auto measure1 = this->measure.measure(fromString, fromString);
    EXPECT_EQ(measure1, 1);
  }
}

TYPED_TEST(MeasureTest, MeasureEquality) {
  for (size_t i = 0; i < this->testJSON.size(); ++i) {
    for (size_t j = 0; j < this->testJSON.size(); ++j) {
      auto lhs = this->getRepFromString(this->testJSON[i]);
      auto lhsDoc = this->getDoc(this->testJSON[i]);

      auto rhs = this->getRepFromString(this->testJSON[j]);
      auto rhsDoc = this->getDoc(this->testJSON[j]);

      auto measure1 = this->measure.measure(lhs, rhs);
      auto measure2 = this->measure.measure(lhsDoc, rhsDoc);
      EXPECT_EQ(measure1, measure2);
    }
  }
}
