//
// Created by Nico on 05/03/2020.
//

#ifndef JODA_SYSTEMREQUEST_H
#define JODA_SYSTEMREQUEST_H

#include <httplib.h>
#include "joda/network/JodaServer.h"

namespace joda::network::apiv2 {

/**
* SystemRequest is responsible for retrieving information about the host system over the API
*/
class SystemRequest {
 public:
  /**
  * Registers this API endpoint at the server
  * @param prefix a prefix to prepend to the http endpoint
  * @param server the server to register the endpoint at
  */
  static void registerEndpoint(const std::string &prefix, httplib::Server &server);

  /**
  * Creates a handler capable of handling a request and return an answer
  * @param prefix a prefix to prepend to the http endpoint (for logging purposes)
  */
  static httplib::Server::Handler getHandler(const std::string prefix = "");
 private:
  static constexpr auto endpoint = "/system";

  static void sendSystemStats(const httplib::Request &req, httplib::Response &res);

};

}

#endif //JODA_SYSTEMREQUEST_H
