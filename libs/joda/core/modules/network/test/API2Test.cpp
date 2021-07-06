#include <gtest/gtest.h>
#include <httplib.h>
#include <joda/network/apiv2/API.h>
#include <joda/storage/collection/StorageCollection.h>
#include "../src/apiv2/DeleteRequest.h"
#include "../src/apiv2/SystemRequest.h"
#include "gmock/gmock.h"

class API2Test : public testing::Test {
 protected:
  httplib::Response resp;
  httplib::Request req;
  void SetUp() override {
    resp = httplib::Response();
    req = httplib::Request();
  }

  testing::AssertionResult respIsError(
      const joda::network::JodaAPIException& except) {
    return respIsError(except.what());
  }
  testing::AssertionResult respIsError(const std::string& errmsg = "") {
    RJDocument doc;
    doc.Parse(resp.body);
    if (doc.HasParseError())
      return testing::AssertionFailure() << "Could not parse response";
    if (!doc.IsObject() || !doc.HasMember("error"))
      return testing::AssertionFailure() << "Response not an error";
    if (errmsg.empty()) return testing::AssertionSuccess();

    if (!doc["error"].IsString())
      return testing::AssertionFailure() << "Error not a string message";
    if (std::string(doc["error"].GetString()) != errmsg)
      return testing::AssertionFailure()
             << "Different error message. Expected '" << errmsg << +"' got '"
             << doc["error"].GetString() << "'";
    return testing::AssertionSuccess();
  }

  testing::AssertionResult respIsEqual(const std::string& expected) {
    if (resp.body != expected)
      return testing::AssertionFailure()
             << "Unexpected Response. Expected '" << expected << +"' got '"
             << resp.body << "'";
    return testing::AssertionSuccess();
  }
};

TEST_F(API2Test, Version) { EXPECT_EQ(joda::network::apiv2::API::VERSION, 2); }

TEST_F(API2Test, Prefix) {
  EXPECT_STRCASEEQ(joda::network::apiv2::API::prefix, "/v2");
}

//////////////////////////////////////
//          Delete
//////////////////////////////////////

TEST_F(API2Test, DeleteNoParams) {
  joda::network::apiv2::DeleteRequest del;
  auto handler = del.getHandler();

  ASSERT_NO_THROW(handler(req, resp));
  EXPECT_TRUE(respIsError(
      joda::network::JodaMissingParameterException("name or result")));
}

TEST_F(API2Test, DeleteNegativeNumber) {
  joda::network::apiv2::DeleteRequest del;
  auto handler = del.getHandler();

  req.params.insert({"result", "-1"});
  ASSERT_NO_THROW(handler(req, resp));
  EXPECT_TRUE(respIsError(joda::network::JodaInvalidParameterException(
      "result", "Cannot be parsed to unsigned long")));
}

TEST_F(API2Test, DeleteResult) {
  joda::network::apiv2::DeleteRequest del;
  auto handler = del.getHandler();

  ASSERT_EQ(StorageCollection::getInstance().getTemporaryStorages().size(), 0);
  auto id = StorageCollection::getInstance().addTemporaryStorage(
      std::make_shared<JSONStorage>("Test"));
  ASSERT_EQ(StorageCollection::getInstance().getTemporaryStorages().size(), 1);

  req.params.insert({"result", std::to_string(id)});
  ASSERT_NO_THROW(handler(req, resp));
  ASSERT_FALSE(respIsError());
  ASSERT_TRUE(respIsEqual("true"));
  ASSERT_EQ(StorageCollection::getInstance().getTemporaryStorages().size(), 0);
}

TEST_F(API2Test, DeleteName) {
  joda::network::apiv2::DeleteRequest del;
  auto handler = del.getHandler();

  ASSERT_EQ(StorageCollection::getInstance().getStorage("Test"), nullptr);
  auto testStorage = StorageCollection::getInstance().getOrAddStorage("Test");
  ASSERT_EQ(StorageCollection::getInstance().getStorage("Test"), testStorage);

  req.params.insert({"name", "Test"});
  ASSERT_NO_THROW(handler(req, resp));
  ASSERT_FALSE(respIsError());
  ASSERT_TRUE(respIsEqual("true"));

  ASSERT_EQ(StorageCollection::getInstance().getStorage("Test"), nullptr);
}

//////////////////////////////////////
//          System
//////////////////////////////////////

TEST_F(API2Test, System) {
  joda::network::apiv2::SystemRequest reqProvider;
  auto handler = reqProvider.getHandler();

  ASSERT_NO_THROW(handler(req, resp));

  RJDocument doc;
  doc.Parse(resp.body);
  ASSERT_FALSE(doc.HasParseError());
  ASSERT_TRUE(doc.IsObject());
  // Memory
  ASSERT_TRUE(doc.HasMember("memory"));
  const auto& memory = doc["memory"];
  ASSERT_TRUE(memory.IsObject());
  EXPECT_EQ(memory.MemberCount(), 5);
  EXPECT_TRUE(memory.HasMember("total"));
  EXPECT_TRUE(memory.HasMember("used"));
  EXPECT_TRUE(memory.HasMember("joda"));
  EXPECT_TRUE(memory.HasMember("allowed_memory"));
  EXPECT_TRUE(memory.HasMember("calculated_memory"));
  // Version
  ASSERT_TRUE(doc.HasMember("version"));
  const auto& version = doc["version"];
  ASSERT_TRUE(version.IsObject());
  ASSERT_EQ(version.MemberCount(), 4);
  EXPECT_TRUE(version.HasMember("version"));
  EXPECT_TRUE(version.HasMember("api"));
  EXPECT_TRUE(version.HasMember("commit"));
  EXPECT_TRUE(version.HasMember("build-time"));
  // Host
  ASSERT_TRUE(doc.HasMember("host"));
  const auto& host = doc["host"];
  ASSERT_TRUE(host.IsObject());
  ASSERT_EQ(host.MemberCount(), 2);
  EXPECT_TRUE(host.HasMember("kernel"));
  EXPECT_TRUE(host.HasMember("os"));
}