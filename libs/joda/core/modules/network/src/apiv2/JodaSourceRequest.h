//
// Created by Nico on 03/03/2020.
//

#ifndef JODA_JODASOURCEREQUEST_H
#define JODA_JODASOURCEREQUEST_H
#include <httplib.h>
#include <joda/storage/JSONStorage.h>
#include <string>

namespace joda::network::apiv2 {

/**
 * JodaSourceRequest is responsible for handling the retrieval of a list of
 * collections over the API
 */
class JodaSourceRequest {
 public:
  /**
   * Registers this API endpoint at the server
   * @param prefix a prefix to prepend to the http endpoint
   * @param server the server to register the endpoint at
   */
  static void registerEndpoint(const std::string &prefix,
                               httplib::Server &server);

 private:
  static constexpr auto endpoint = "/sources";
  static constexpr auto temp_endpoint = "/results";

  static void sendSources(const httplib::Request &req, httplib::Response &res);
  static void sendTemporaries(const httplib::Request &req,
                              httplib::Response &res);
  static std::string storageToJSON(const JSONStorage &storage,
                                   unsigned long id = 0);
};

}  // namespace joda::network::apiv2

#endif  // JODA_JODASOURCEREQUEST_H
