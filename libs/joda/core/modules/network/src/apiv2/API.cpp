//
// Created by Nico on 23/08/2019.
//

#include "joda/network/apiv2/API.h"
#include "DeleteRequest.h"
#include "JodaQueryRequest.h"
#include "JodaResultRequest.h"
#include "JodaSourceRequest.h"
#include "SystemRequest.h"
#include "ModulePost.h"
#include "ModuleRequest.h"

void joda::network::apiv2::API::registerEndpoint(const std::string& prefix,
                                                 httplib::Server& server) {
  JodaQueryRequest::registerEndpoint((prefix + API::prefix), server);
  JodaResultRequest::registerEndpoint((prefix + API::prefix), server);
  JodaSourceRequest::registerEndpoint((prefix + API::prefix), server);
  DeleteRequest::registerEndpoint((prefix + API::prefix), server);
  SystemRequest::registerEndpoint((prefix + API::prefix), server);
  ModulePost::registerEndpoint((prefix + API::prefix), server);
  ModuleRequest::registerEndpoint((prefix + API::prefix), server);
}
