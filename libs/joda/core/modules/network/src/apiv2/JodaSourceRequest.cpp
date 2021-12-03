//
// Created by Nico on 03/03/2020.
//

#include "JodaSourceRequest.h"
#include <joda/misc/MemoryUtility.h>
#include <joda/network/JodaServer.h>
#include <joda/storage/collection/StorageCollection.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

void joda::network::apiv2::JodaSourceRequest::registerEndpoint(
    const std::string& prefix, httplib::Server& server) {
  auto fullEndpoint = prefix + endpoint;
  server.Get(fullEndpoint.c_str(), [fullEndpoint](const httplib::Request& req,
                                                  httplib::Response& res) {
    try {
      JodaServer::logExecutionTime(fullEndpoint,
                                   [&]() { sendSources(req, res); });
    } catch (JodaAPIException& e) {
      JodaServer::handleError(e, res);
    }
  });

  server.Post(fullEndpoint.c_str(), [fullEndpoint](const httplib::Request& req,
                                                   httplib::Response& res) {
    try {
      JodaServer::logExecutionTime(fullEndpoint,
                                   [&]() { sendSources(req, res); });
    } catch (JodaAPIException& e) {
      JodaServer::handleError(e, res);
    }
  });

  server.Get(
      (prefix + temp_endpoint).c_str(),
      [fullEndpoint](const httplib::Request& req, httplib::Response& res) {
        try {
          JodaServer::logExecutionTime(fullEndpoint,
                                       [&]() { sendTemporaries(req, res); });
        } catch (JodaAPIException& e) {
          JodaServer::handleError(e, res);
        }
      });

  server.Post(
      (prefix + temp_endpoint).c_str(),
      [fullEndpoint](const httplib::Request& req, httplib::Response& res) {
        try {
          JodaServer::logExecutionTime(fullEndpoint,
                                       [&]() { sendTemporaries(req, res); });
        } catch (JodaAPIException& e) {
          JodaServer::handleError(e, res);
        }
      });
}

void joda::network::apiv2::JodaSourceRequest::sendSources(
    const httplib::Request& /*req*/, httplib::Response& res) {
  res.set_header("charset", "utf-8");
  res.set_chunked_content_provider(
      "application/json", [](uint64_t offset_, httplib::DataSink& out) {
        out.write("[", 1);
        auto storages = StorageCollection::getInstance().getStorages();
        for (size_t i = 0; i < storages.size(); ++i) {
          auto str = storageToJSON(*storages[i]);
          out.write(str.c_str(), str.size());
          if (i != storages.size() - 1) {
            out.write(",", 1);
          }
        }
        out.write("]", 1);
        out.done();
        return true;
      });
}

void joda::network::apiv2::JodaSourceRequest::sendTemporaries(
    const httplib::Request& /*req*/, httplib::Response& res) {
  res.set_header("charset", "utf-8");
  res.set_chunked_content_provider(
      "application/json", [](uint64_t offset_, httplib::DataSink& out) {
        out.write("[", 1);
        auto storages =
            StorageCollection::getInstance().getTemporaryIDStorages();
        for (size_t i = 0; i < storages.size(); ++i) {
          auto str = storageToJSON(*storages[i].second, storages[i].first);
          out.write(str.c_str(), str.size());
          if (i != storages.size() - 1) {
            out.write(",", 1);
          }
        }
        out.write("]", 1);
        out.done();
        return true;
      });
}

std::string joda::network::apiv2::JodaSourceRequest::storageToJSON(
    const JSONStorage& storage, unsigned long id) {
  auto doc = RJDocument(rapidjson::kObjectType);

  if (id >= JODA_STORE_VALID_ID_START) {
    doc.AddMember("id", id, doc.GetAllocator());
  } else {
    doc.AddMember("name", storage.getName(), doc.GetAllocator());
  }
  doc.AddMember("query", storage.getQueryString(), doc.GetAllocator());
  doc.AddMember("documents",storage.size(), doc.GetAllocator());
  doc.AddMember("container", storage.contSize(), doc.GetAllocator());
  doc.AddMember("memory", storage.estimatedSize(), doc.GetAllocator());
  doc.AddMember("memory-str",MemoryUtility::MemorySize(storage.estimatedSize()).getHumanReadable(), doc.GetAllocator());

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  return buffer.GetString();
}
