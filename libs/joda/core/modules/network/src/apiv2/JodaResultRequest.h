//
// Created by Nico on 23/08/2019.
//

#ifndef JODA_JODARESULTREQUEST_H
#define JODA_JODARESULTREQUEST_H
#include <string>
#include <httplib.h>
#include <joda/storage/JSONStorage.h>

namespace joda::network::apiv2 {

/**
* JodaResultRequest is responsible for handling the retrieval of result documents over the API
*/
class JodaResultRequest {
 public:
  /**
  * Registers this API endpoint at the server
  * @param prefix a prefix to prepend to the http endpoint
  * @param server the server to register the endpoint at
  */
  static void registerEndpoint(const std::string &prefix, httplib::Server &server);

 private:
  static constexpr auto endpoint = "/result";
  static constexpr auto chunkSize = 50;

  static void result(const httplib::Request &req, httplib::Response &res);

  static void sendResult(httplib::Response &res,
                         const std::shared_ptr<JSONStorage> &result,
                         long long offset,
                         long long count);
};

}

#endif //JODA_JODARESULTREQUEST_H
