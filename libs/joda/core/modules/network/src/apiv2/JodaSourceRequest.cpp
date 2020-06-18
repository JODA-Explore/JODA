//
// Created by Nico on 03/03/2020.
//

#include <joda/network/JodaServer.h>
#include <joda/storage/collection/StorageCollection.h>
#include <joda/misc/MemoryUtility.h>
#include "JodaSourceRequest.h"

void joda::network::apiv2::JodaSourceRequest::registerEndpoint(const std::string &prefix, httplib::Server &server) {
  auto fullEndpoint = prefix + endpoint;
  server.Get(fullEndpoint.c_str(), [fullEndpoint](const httplib::Request &req, httplib::Response &res) {
    try {
      JodaServer::logExecutionTime(fullEndpoint, [&]() { sendSources(req, res); });
    } catch (JodaAPIException &e) {
      JodaServer::handleError(e, res);
    }
  });

  server.Post(fullEndpoint.c_str(), [fullEndpoint](const httplib::Request &req, httplib::Response &res) {
    try {
      JodaServer::logExecutionTime(fullEndpoint, [&]() { sendSources(req, res); });
    } catch (JodaAPIException &e) {
      JodaServer::handleError(e, res);
    }
  });

  server.Get((prefix + temp_endpoint).c_str(), [fullEndpoint](const httplib::Request &req, httplib::Response &res) {
    try {
      JodaServer::logExecutionTime(fullEndpoint, [&]() { sendTemporaries(req, res); });
    } catch (JodaAPIException &e) {
      JodaServer::handleError(e, res);
    }
  });

  server.Post((prefix + temp_endpoint).c_str(), [fullEndpoint](const httplib::Request &req, httplib::Response &res) {
    try {
      JodaServer::logExecutionTime(fullEndpoint, [&]() { sendTemporaries(req, res); });
    } catch (JodaAPIException &e) {
      JodaServer::handleError(e, res);
    }
  });
}

void joda::network::apiv2::JodaSourceRequest::sendSources(const httplib::Request &req, httplib::Response &res) {
  res.set_header("Content-Type", "application/json");
  res.set_header("charset", "utf-8");
  res.set_chunked_content_provider([](uint64_t offset_,
                                      httplib::DataSink out,
                                      httplib::Done done) {
    out("[", 1);
    auto storages = StorageCollection::getInstance().getStorages();
    for (int i = 0; i < storages.size(); ++i) {
      auto str = storageToJSON(*storages[i]);
      out(str.c_str(), str.size());
      if (i != storages.size() - 1) {
        out(",", 1);
      }
    }
    out("]", 1);
    done();
  });
}

void joda::network::apiv2::JodaSourceRequest::sendTemporaries(const httplib::Request &req, httplib::Response &res) {
  res.set_header("Content-Type", "application/json");
  res.set_header("charset", "utf-8");
  res.set_chunked_content_provider([](uint64_t offset_,
                                      httplib::DataSink out,
                                      httplib::Done done) {
    out("[", 1);
    auto storages = StorageCollection::getInstance().getTemporaryIDStorages();
    for (int i = 0; i < storages.size(); ++i) {
      auto str = storageToJSON(*storages[i].second, storages[i].first);
      out(str.c_str(), str.size());
      if (i != storages.size() - 1) {
        out(",", 1);
      }
    }
    out("]", 1);
    done();
  });
}

std::string joda::network::apiv2::JodaSourceRequest::storageToJSON(const JSONStorage &storage, unsigned long id) {
  std::string ret = "{";
  if (id >= JODA_STORE_VALID_ID_START) {
    ret += R"("id":)" + std::to_string(id) + ",";
  } else {
    ret += R"("name":")" + storage.getName() + "\",";
  }
  ret += R"("documents":)" + std::to_string(storage.size()) + ",";
  ret += R"("container":)" + std::to_string(storage.contSize()) + ",";
  ret += R"("memory":)" + std::to_string(storage.estimatedSize()) + ",";
  ret += R"("memory-str":")" +  MemoryUtility::MemorySize(storage.estimatedSize()).getHumanReadable() + "\"";
  ret += "}";
  return ret;
}
