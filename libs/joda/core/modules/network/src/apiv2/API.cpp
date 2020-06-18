//
// Created by Nico on 23/08/2019.
//

#include "joda/network/apiv2/API.h"
#include "JodaQueryRequest.h"
#include "JodaResultRequest.h"
#include "JodaSourceRequest.h"
#include "DeleteRequest.h"
#include "SystemRequest.h"

void joda::network::apiv2::API::registerEndpoint(const std::string &prefix, httplib::Server &server) {
  JodaQueryRequest::registerEndpoint((prefix + API::prefix), server);
  JodaResultRequest::registerEndpoint((prefix + API::prefix), server);
  JodaSourceRequest::registerEndpoint((prefix + API::prefix), server);
  DeleteRequest::registerEndpoint((prefix + API::prefix), server);
  SystemRequest::registerEndpoint((prefix + API::prefix), server);
}

constexpr size_t joda::network::apiv2::API::VERSION;
constexpr const char *joda::network::apiv2::API::prefix;