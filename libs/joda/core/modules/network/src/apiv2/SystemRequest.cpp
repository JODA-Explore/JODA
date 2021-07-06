//
// Created by Nico on 05/03/2020.
//

#include "SystemRequest.h"
#include <joda/config/config.h>
#include <joda/misc/MemoryUtility.h>
#include <joda/misc/OSUtility.h>
#include <joda/storage/collection/StorageCollection.h>
#include <joda/version.h>

httplib::Server::Handler joda::network::apiv2::SystemRequest::getHandler(
    const std::string& prefix) {
  auto fullEndpoint = prefix + endpoint;
  return [fullEndpoint](const httplib::Request& req, httplib::Response& res) {
    try {
      JodaServer::logExecutionTime(fullEndpoint,
                                   [&]() { sendSystemStats(req, res); });
    } catch (JodaAPIException& e) {
      JodaServer::handleError(e, res);
    }
  };
}

void joda::network::apiv2::SystemRequest::registerEndpoint(
    const std::string& prefix, httplib::Server& server) {
  auto fullEndpoint = prefix + endpoint;
  server.Get(fullEndpoint.c_str(), getHandler(prefix));
  server.Post(fullEndpoint.c_str(), getHandler(prefix));
}

void joda::network::apiv2::SystemRequest::sendSystemStats(
    const httplib::Request& /*req*/, httplib::Response& res) {
  RJDocument ret(rapidjson::kObjectType);

  // Memory consumption
  RJValue memory(rapidjson::kObjectType);
  memory.AddMember("total",
                   static_cast<uint64_t>(MemoryUtility::totalRam().getBytes()),
                   ret.GetAllocator());
  memory.AddMember(
      "used", static_cast<uint64_t>(MemoryUtility::sysRamUsage().getBytes()),
      ret.GetAllocator());
  memory.AddMember(
      "joda", static_cast<uint64_t>(MemoryUtility::procRamUsage().getBytes()),
      ret.GetAllocator());
  memory.AddMember("allowed_memory", static_cast<uint64_t>(config::maxmemory),
                   ret.GetAllocator());
  memory.AddMember("calculated_memory",
                   static_cast<uint64_t>(
                       StorageCollection::getInstance().estimatedStorageSize()),
                   ret.GetAllocator());

  ret.AddMember("memory", memory, ret.GetAllocator());

  // Versions
  RJValue version(rapidjson::kObjectType);
  version.AddMember("version", JODA_VERSION_STRING, ret.GetAllocator());
  version.AddMember("api", JODA_API_VERSION, ret.GetAllocator());
  version.AddMember("commit", JODA_GIT_REVISION, ret.GetAllocator());
  version.AddMember("build-time", JODA_BUILD_TIME, ret.GetAllocator());
  ret.AddMember("version", version, ret.GetAllocator());

  // Host
  RJValue host(rapidjson::kObjectType);
  host.AddMember("kernel", OSUtility::getKernel(), ret.GetAllocator());
  host.AddMember("os", OSUtility::getOS(), ret.GetAllocator());
  ret.AddMember("host", host, ret.GetAllocator());

  JodaServer::sendResponse(ret, res);
}
