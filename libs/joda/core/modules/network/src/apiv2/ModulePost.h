#ifndef JODA_MODULE_POST_H
#define JODA_MODULE_POST_H
#include <httplib.h>
#include <joda/storage/JSONStorage.h>
#include <string>
#include <filesystem>

namespace joda::network::apiv2 {

/**
 * ModulePost is responsible for receiving and storing modules
 */
class ModulePost {
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

  static void store_module(const httplib::Request &req, httplib::Response &res);

  static std::filesystem::path ensure_directory(const std::string &name);
};

}  // namespace joda::network::apiv2

#endif  // JODA_MODULE_POST_H
