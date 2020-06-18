//
// Created by Nico on 23/08/2019.
//

#ifndef JODA_RESULTREQUEST_H
#define JODA_RESULTREQUEST_H

#include <httplib.h>

namespace joda::network::client {
class ResultRequest {
 public:
  static std::vector<std::string> result(httplib::Client &client,
                                         const std::string &prefix,
                                         unsigned int id,
                                         bool pretty = false);
  static std::vector<std::string> result(httplib::Client &client,
                                         const std::string &prefix,
                                         unsigned int id,
                                         unsigned int offset,
                                         unsigned int count,
                                         bool pretty = false);
  static constexpr auto endpoint = "/result";

 private:
  static std::vector<std::string> result(httplib::Client &client, const std::string &prefix, httplib::Params &params,
                                         bool pretty = false);
};
}

#endif //JODA_RESULTREQUEST_H
