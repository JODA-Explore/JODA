//
// Created by Nico on 23/08/2019.
//

#ifndef JODA_API_H
#define JODA_API_H

#include <httplib.h>
#include "joda/network/JodaServer.h"
namespace joda::network::apiv2 {

/**
 * JODA network API version V2
 */
class API {
 public:
  /**
   * Numerical representation of the API version
   */
  static constexpr size_t VERSION = 2;

  /**
   * The endpoint prefix where this API version can be reached
   */
  static constexpr auto prefix = "/v2";

  /**
   * Registers all API endpoints at the server
   * @param prefix a prefix to prepend to each http endpoint
   * @param server the server to register the endpoints at
   */
  static void registerEndpoint(const std::string &prefix,
                               httplib::Server &server);
};

/**
 * Exception describing API version mismatches.
 */
class JodaAPIVersionException : public JodaAPIException {
 public:
  JodaAPIVersionException() : JodaAPIException("Missing 'API' parameter") {}

  explicit JodaAPIVersionException(unsigned int got)
      : JodaAPIException("Expected version " + std::to_string(API::VERSION) +
                         " but got " + std::to_string(got)) {}

  explicit JodaAPIVersionException(const std::string &what)
      : JodaAPIException("'API' parameter invalid: " + what) {}
};

}  // namespace joda::network::apiv2
#endif  // JODA_API_H
