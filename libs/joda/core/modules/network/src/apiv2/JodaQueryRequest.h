
//
// Created by Nico on 22/08/2019.
//

#ifndef JODA_JODAQUERYREQUEST_H
#define JODA_JODAQUERYREQUEST_H

#include <httplib.h>
#include <joda/misc/Benchmark.h>
#include "joda/network/JodaServer.h"

namespace joda::network::apiv2 {

/**
 * JodaQueryRequest is responsible for handling the execution of queries over
 * the API
 */
class JodaQueryRequest {
 public:
  /**
   * Registers this API endpoint at the server
   * @param prefix a prefix to prepend to the http endpoint
   * @param server the server to register the endpoint at
   */
  static void registerEndpoint(const std::string &prefix,
                               httplib::Server &server);

 private:
  static constexpr auto endpoint = "/query";

  static void query(const httplib::Request &req, httplib::Response &res);
  static void success_(unsigned long result, httplib::Response &res,
                       const Benchmark &benchmark);
  static void export_(httplib::Response &res);
  static void empty_(httplib::Response &res);
  static void skipped_(httplib::Response &res);
  static void error_(unsigned long code, httplib::Response &res);
  static RJDocument successDocument(unsigned long result, int64_t size,
                                    const std::string &message,
                                    const Benchmark * = nullptr);

  static std::vector<std::string> splitQueries(const std::string &query);
};

class JodaQueryException : public JodaAPIException {
 public:
  JodaQueryException() : JodaAPIException("Missing 'Query' parameter") {}

  explicit JodaQueryException(const std::string &what)
      : JodaAPIException("'Query' parameter invalid: " + what) {}
};

class JodaQueryUnkownErrorException : public JodaAPIException {
 public:
  JodaQueryUnkownErrorException()
      : JodaAPIException(
            "A unknown error happened, check server logs for information.") {}
};

class JodaQuerySkippedException : public JodaAPIException {
 public:
  JodaQuerySkippedException()
      : JodaAPIException(
            "The server skipped the query because of an error. Check logs") {}
};
}  // namespace joda::network::apiv2

#endif  // JODA_JODAQUERYREQUEST_H
