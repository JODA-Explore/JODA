//
// Created by Nico on 04/03/2020.
//

#include "DeleteRequest.h"
#include <joda/storage/collection/StorageCollection.h>

void joda::network::apiv2::DeleteRequest::registerEndpoint(
    const std::string& prefix, httplib::Server& server) {
  auto fullEndpoint = prefix + endpoint;
  server.Get(fullEndpoint.c_str(), getHandler(prefix));
  server.Post(fullEndpoint.c_str(), getHandler(prefix));
}

void joda::network::apiv2::DeleteRequest::readParams(
    const httplib::Request& req, httplib::Response& res) {
  auto nameParam = req.params.find("name");
  auto resultParam = req.params.find("result");
  if (nameParam != req.params.end()) {
    return deleteSource(req, res, nameParam->second);
  }
  if (resultParam != req.params.end()) {
    unsigned long id = 0;
    try {
      if (strtod(resultParam->second.c_str(), nullptr) < 0) {
        throw std::out_of_range("Negative number");
      }
      id = std::stoul(resultParam->second);
    } catch (std::exception& e) {
      throw(JodaInvalidParameterException("result",
                                          "Cannot be parsed to unsigned long"));
    }
    return deleteResult(req, res, id);
  } else {
    throw JodaMissingParameterException("name or result");
  }
}

void joda::network::apiv2::DeleteRequest::deleteSource(
    const httplib::Request& /*req*/, httplib::Response& res,
    const std::string& name) {
  StorageCollection::getInstance().removeStorage(name);
  return success_(res);
}

void joda::network::apiv2::DeleteRequest::deleteResult(
    const httplib::Request& /*req*/, httplib::Response& res, unsigned long id) {
  StorageCollection::getInstance().removeStorage(id);
  return success_(res);
}

void joda::network::apiv2::DeleteRequest::success_(httplib::Response& res) {
  RJDocument doc(rapidjson::kTrueType);
  JodaServer::sendResponse(doc, res);
}

httplib::Server::Handler joda::network::apiv2::DeleteRequest::getHandler(
    const std::string& prefix) {
  auto fullEndpoint = prefix + endpoint;
  return [fullEndpoint](const httplib::Request& req, httplib::Response& res) {
    try {
      JodaServer::logExecutionTime(fullEndpoint,
                                   [&]() { readParams(req, res); });
    } catch (JodaAPIException& e) {
      JodaServer::handleError(e, res);
    }
  };
}
