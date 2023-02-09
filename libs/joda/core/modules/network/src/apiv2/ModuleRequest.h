#ifndef JODA_MODULEREQUEST_H
#define JODA_MODULEREQUEST_H
#include <httplib.h>
#include <string>
#include <joda/extension/ModuleRegister.h>

namespace joda::network::apiv2 {

/**
 * ModuleRequest is responsible for handling the retrieval of a list of
 * collections over the API
 */
class ModuleRequest {
 public:
  /**
   * Registers this API endpoint at the server
   * @param prefix a prefix to prepend to the http endpoint
   * @param server the server to register the endpoint at
   */
  static void registerEndpoint(const std::string &prefix,
                               httplib::Server &server);

 private:
  static constexpr auto endpoint = "/module";

  static void sendModules(const httplib::Request &req, httplib::Response &res);

  static std::string moduleToJSON(const extension::ModuleSummary &summary);
};

}  // namespace joda::network::apiv2

#endif  // JODA_MODULEREQUEST_H
