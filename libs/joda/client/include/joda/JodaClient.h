//
// Created by Nico Schäfer on 19/12/17.
//

#ifndef JODA_JODACLIENT_H
#define JODA_JODACLIENT_H
#include <string>
#include <httplib.h>
#include <rapidjson/document.h>
#include "../../../../../extern/cpp-httplib/httplib.h"

namespace joda::network::client {
#define JODA_CLIENT_NON_INTERACTIVE_BATCH_SIZE 5000
#define JODA_CLIENT_TIMEOUT 600

class JodaClientException : public std::runtime_error {
 public:
  explicit JodaClientException(const std::string &what) : runtime_error("Client Error: " + what) {};
};

class JodaServerException : public std::runtime_error {
 public:
  explicit JodaServerException(const std::string &what) : runtime_error("Server Error: " + what) {};
};

class JodaClient {
 public:
  JodaClient(const std::string &addr, int port);
  //Interactive
  void cli();
  //Non-Interactive
  void nonInteractiveQuery(const std::string &query, unsigned long offset, unsigned long count);

  static rapidjson::Document parseResponse(httplib::Response &res);
  static void checkError(const rapidjson::Document &doc);
  static constexpr size_t VERSION = 2;

 protected:
  std::string query;
  httplib::Client client;

  static constexpr auto prefix = "/api/v2";

  static void handleError(JodaClientException &e);
  static void handleError(JodaServerException &e);




  std::string &ltrim(std::string &str);
  std::string &rtrim(std::string &str);
  std::string parseCommand();

  static void getStringWindowSize(const std::string &str, int &y, int &x);
  void browseReturn(unsigned long id, unsigned long max);
};

class JodaClientNoResponseException : public JodaClientException {
 public:
  explicit JodaClientNoResponseException() : JodaClientException("Got no response, server online?") {};
};

class JodaServerHTTPException : public JodaServerException {
 public:
  explicit JodaServerHTTPException(int status) : JodaServerException("Got status " + std::to_string(status)) {};
};

class JodaServerAPIErrorException : public JodaServerException {
 public:
  explicit JodaServerAPIErrorException(const std::string &what) : JodaServerException("API returned error: " + what) {};
};

class JodaServerInvalidJSONException : public JodaServerException {
 public:
  explicit JodaServerInvalidJSONException(const std::string &what) : JodaServerException(
      "Could not parse response: " + what) {};
};
}

#endif //JODA_JODACLIENT_H
