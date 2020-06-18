//
// Created by Nico on 04/03/2020.
//

#ifndef JODA_DELETEREQUEST_H
#define JODA_DELETEREQUEST_H

#include <httplib.h>
#include "joda/network/JodaServer.h"

namespace joda::network::apiv2 {

/**
* DeleteRequest is responsible for handling deletion of collections over the API
*/
class DeleteRequest {
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
  static constexpr auto endpoint = "/delete";

  static void readParams(const httplib::Request &req, httplib::Response &res);
  static void deleteSource(const httplib::Request &req, httplib::Response &res, const std::string &name);
  static void deleteResult(const httplib::Request &req, httplib::Response &res, unsigned long id);

  static void success_(httplib::Response &res);

};

}

#endif //JODA_DELETEREQUEST_H
