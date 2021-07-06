//
// Created by Nico on 23/08/2019.
//

#ifndef JODA_QUERYREQUEST_H
#define JODA_QUERYREQUEST_H
#include <httplib.h>
#include <string>

namespace joda::network::client {
class QueryRequest {
 public:
  static std::pair<int64_t, int64_t> query(httplib::Client &client,
                                           const std::string &query,
                                           const std::string &prefix);
  static constexpr auto endpoint = "/query";
};
}  // namespace joda::network::client

#endif  // JODA_QUERYREQUEST_H
