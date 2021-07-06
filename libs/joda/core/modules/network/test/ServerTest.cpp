#define CPPHTTPLIB_THREAD_POOL_COUNT 1

#include <gtest/gtest.h>
#include <joda/misc/RJFwd.h>
#include <joda/network/JodaServer.h>
#include <future>
#include "../src/Favicon.h"

class ServerTest : public testing::Test {
 protected:
  joda::network::JodaServer server;
  std::thread serverThread;
  static std::mutex testLock;

  void SetUp() override {
    std::lock_guard<std::mutex> guard(testLock);
    serverThread = std::thread([&]() {
      ASSERT_TRUE(server.start("localhost", 5632))
          << "Could not start server, busy port?";
    });
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  void TearDown() override {
    std::lock_guard<std::mutex> guard(testLock);
    server.stop();
    serverThread.join();
  }
};

std::mutex ServerTest::testLock{};

TEST_F(ServerTest, Response) {
  httplib::Response resp;
  std::string responseStr = R"({"Test":"Test"})";
  RJDocument responseObj;
  responseObj.Parse(responseStr);
  server.sendResponse(responseObj, resp);

  EXPECT_STREQ(resp.get_header_value("Content-Type").c_str(),
               "application/json");

  EXPECT_STREQ(resp.body.c_str(), responseStr.c_str());
}

TEST_F(ServerTest, ErrorHandling) {
  joda::network::JodaAPIException ex("Test");
  httplib::Response resp;
  server.handleError(ex, resp);

  EXPECT_STREQ(resp.get_header_value("Content-Type").c_str(),
               "application/json");
  std::string msg = std::string(R"({"error":")");
  msg += ex.what();
  msg += R"("})";
  EXPECT_STREQ(resp.body.c_str(), msg.c_str());
}

TEST_F(ServerTest, Favicon) {
  httplib::Client client("localhost", 5632);
  ASSERT_TRUE(client.is_valid());

  auto response = client.Get("/favicon.ico");

  ASSERT_TRUE(response != nullptr);

  EXPECT_STREQ(response->get_header_value("Content-Type").c_str(), "image/ico");

  EXPECT_STREQ(response->body.c_str(),
               reinterpret_cast<const char*>(&joda::network::JODA_FAVICON[0]));
}
