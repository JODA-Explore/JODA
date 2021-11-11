//
// Created by Nico on 22/08/2019.
//

#include "joda/network/JodaServer.h"
#include "joda/network/apiv2/API.h"

#include <joda/misc/RJFwd.h>
#include "Favicon.h"
#include "apiv2/JodaQueryRequest.h"

#include <glog/logging.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

bool joda::network::JodaServer::start(const std::string& addr, int port) {
  /*
   * Error Handler
   */
  server.set_error_handler([](const auto& req, auto& res) {
    const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
    char buf[BUFSIZ];
    snprintf(buf, sizeof(buf), fmt, res.status);
    res.set_content(buf, "text/html");
  });

  /*
   * Log Handler
   */
  server.set_logger([](const auto& req, const auto& res) {
    LOG(INFO) << "Got request: " << req.path << " (Response: " << res.status
              << ")";
    LOG(INFO) << "Request Parameters:\n" << printParameters(req.params);
  });

  /*
   * Endpoint handlers
   */

  apiv2::API::registerEndpoint(prefix, server);
  server.Get("/favicon.ico", favicon);

  LOG(INFO) << "Starting server on " << addr << ":" << port;
  return server.listen(addr.c_str(), port);
}

void joda::network::JodaServer::handleError(JodaAPIException& e,
                                            httplib::Response& res) {
  RJDocument response(rapidjson::kObjectType);
  RJValue err(e.what(), response.GetAllocator());
  response.AddMember("error", err, response.GetAllocator());
  sendResponse(response, res);
  LOG(WARNING) << "Error in request: " << e.what();
}

void joda::network::JodaServer::sendResponse(RJDocument& doc,
                                             httplib::Response& res) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  auto r = buffer.GetString();
  res.set_content(r, "application/json");
}

void joda::network::JodaServer::stop() { server.stop(); }

void joda::network::JodaServer::favicon(const httplib::Request& /*req*/,
                                        httplib::Response& res) {
  res.set_content(reinterpret_cast<const char*>(&JODA_FAVICON[0]),
                  JODA_FAVICON_len, "image/ico");
}

std::string joda::network::JodaServer::printParameters(
    const httplib::Params& p) {
  std::string ret = "{";
  for (const auto& item : p) {
    ret += "\"" + item.first + "\" : \"" + item.second + "\"";
    if (ret.size() > 1) {
      ret += ", ";
    }
  }
  ret += "}";
  return ret;
}
