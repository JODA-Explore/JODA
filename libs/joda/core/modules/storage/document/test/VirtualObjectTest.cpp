
#include <gtest/gtest.h>
#include <joda/misc/RJFwd.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <joda/document/view/VirtualObject.h>

/*
 * ViewTest
 */
class VirtualObjectTest : public ::testing::Test {
 public:

 protected:
 protected:
  virtual void SetUp() {
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

  static std::string stringify(const RJValue &doc) {
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);
    doc.Accept(writer);
    return buff.GetString();
  }

  std::string stringify(VirtualObject &obj) {
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);
    obj.Accept(writer);
    return buff.GetString();
  }

  ViewStructure struc;

};

TEST_F(VirtualObjectTest, EmptyObject) {
  VirtualObject o(&struc);

  EXPECT_EQ(o.size(), 0);

  EXPECT_STRCASEEQ(R"({})", stringify(o).c_str());
}

TEST_F(VirtualObjectTest, AddAtomCopy) {
  VirtualObject o(&struc);

  EXPECT_EQ(o.size(), 0);
  o.reserve(1);
  EXPECT_EQ(o.size(), 0);
  EXPECT_STRCASEEQ(R"({})", stringify(o).c_str());

  RJValue val(1);
  const std::string name = "A";
  o.addMember(struc.getOrAdd(name), &val);
  EXPECT_EQ(o.size(), 1);
  EXPECT_STRCASEEQ(R"({"A":1})", stringify(o).c_str());
}

TEST_F(VirtualObjectTest, AddNestedCopy) {
  VirtualObject o(&struc);

  EXPECT_EQ(o.size(), 0);
  o.reserve(1);
  EXPECT_EQ(o.size(), 0);
  EXPECT_STRCASEEQ(R"({})", stringify(o).c_str());

  VirtualObject o2(&struc);
  EXPECT_EQ(o2.size(), 0);
  o2.reserve(1);
  EXPECT_EQ(o2.size(), 0);
  const std::string name = "A";
  o.addMember(struc.getOrAdd(name), &o2);
  EXPECT_EQ(o.size(), 1);
  EXPECT_STRCASEEQ(R"({"A":{}})", stringify(o).c_str());
}

TEST_F(VirtualObjectTest, DeepCopy) {
  VirtualObject o(&struc);

  EXPECT_EQ(o.size(), 0);
  o.reserve(1);
  EXPECT_EQ(o.size(), 0);
  EXPECT_STRCASEEQ(R"({})", stringify(o).c_str());

  VirtualObject o2(&struc);
  EXPECT_EQ(o2.size(), 0);
  o2.reserve(1);
  EXPECT_EQ(o2.size(), 0);
  const std::string name = "A";
  o.addMember(struc.getOrAdd(name), &o2);
  EXPECT_EQ(o.size(), 1);
  EXPECT_STRCASEEQ(R"({"A":{}})", stringify(o).c_str());

  RJMemoryPoolAlloc alloc;
  auto val = o.deepCopy(alloc);
  EXPECT_STRCASEEQ(R"({"A":{}})", stringify(val).c_str());
}

TEST_F(VirtualObjectTest, OneAttributeTest) {
  VirtualObject o(&struc);
  o.reserve(1);
  RJValue val(1);
  const std::string name = "A";
  o.addMember(struc.getOrAdd(name), &val);
  auto atts = o.attributes();

  ASSERT_EQ(atts.size(), 1);
  EXPECT_STRCASEEQ(atts.front().c_str(), "A");
}

TEST_F(VirtualObjectTest, TwoAttributesTest) {
  VirtualObject o(&struc);
  o.reserve(2);
  RJValue val(1);
  {
    const std::string name = "A";
    o.addMember(struc.getOrAdd(name), &val);
  }
  {
    const std::string name = "B";
    o.addMember(struc.getOrAdd(name), &val);
  }
  auto atts = o.attributes();

  ASSERT_EQ(atts.size(), 2);
  EXPECT_STRCASEEQ(atts[0].c_str(), "A");
  EXPECT_STRCASEEQ(atts[1].c_str(), "B");
}
