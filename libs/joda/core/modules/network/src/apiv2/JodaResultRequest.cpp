//
// Created by Nico on 23/08/2019.
//

#include "JodaResultRequest.h"
#include <joda/network/JodaServer.h>
#include <joda/storage/collection/StorageCollection.h>

void joda::network::apiv2::JodaResultRequest::registerEndpoint(
    const std::string& prefix, httplib::Server& server) {
  auto fullEndpoint = prefix + endpoint;
  server.Get(fullEndpoint.c_str(), [fullEndpoint](const httplib::Request& req,
                                                  httplib::Response& res) {
    try {
      JodaServer::logExecutionTime(fullEndpoint, [&]() { result(req, res); });
    } catch (JodaAPIException& e) {
      JodaServer::handleError(e, res);
    }
  });

  server.Post(fullEndpoint.c_str(), [fullEndpoint](const httplib::Request& req,
                                                   httplib::Response& res) {
    try {
      JodaServer::logExecutionTime(fullEndpoint, [&]() { result(req, res); });
    } catch (JodaAPIException& e) {
      JodaServer::handleError(e, res);
    }
  });
}

void joda::network::apiv2::JodaResultRequest::result(
    const httplib::Request& req, httplib::Response& res) {
  auto id_ = req.params.find("id");
  if (id_ == req.params.end()) {
    throw(JodaMissingParameterException("id"));
  }

  long long id;
  try {
    id = std::stoll(id_->second);
  } catch (std::exception& e) {
    throw(JodaInvalidParameterException("id", "Cannot be parsed to int"));
  }

  long long offset = 0;
  auto offset_ = req.params.find("offset");
  if (offset_ != req.params.end()) {
    try {
      offset = std::stoll(offset_->second);
    } catch (std::exception& e) {
      throw(JodaInvalidParameterException("offset", "Cannot be parsed to int"));
    }
  }

  long long count = 0;
  auto count_ = req.params.find("count");
  if (count_ != req.params.end()) {
    try {
      count = std::stoll(count_->second);
    } catch (std::exception& e) {
      throw(JodaInvalidParameterException("count", "Cannot be parsed to int"));
    }
  }

  if (id < JODA_STORE_VALID_ID_START) {
    LOG(ERROR) << "Got invalid result id (" << id << ")";
    throw(JodaInvalidParameterException("id", "Invalid result id"));
  }

  auto result = StorageCollection::getInstance().getStorage(id);
  if (result == nullptr) {
    LOG(ERROR) << "Requested non existing result (" << id << ")";
    throw(JodaInvalidParameterException("id", "Result does not exist"));
  }

  sendResult(res, result, offset, count);
}

void joda::network::apiv2::JodaResultRequest::sendResult(
    httplib::Response& res, const std::shared_ptr<JSONStorage>& result,
    long long offset, long long count) {
  if (count == 0) {
    count = result->size();
  } else {
    count = offset + count;
  }

  res.set_header("charset", "utf-8");
  res.set_chunked_content_provider(
      "application/json",
      [offset, count, result](size_t offset_, httplib::DataSink& out) {
        out.write("{\"result\":[", 11);
        auto start = offset;
        auto end = count;

        bool prev = false;
        while (end > start) {
          auto size = std::min(static_cast<long long>(chunkSize), end - start);
          auto stringified = result->stringify(start, start + size - 1);
          for (auto&& str : stringified) {
            if (prev) {
              out.write(",", 1);
            } else {
              prev = true;
            }
            out.write(str.c_str(), str.size());
          }
          start += size;
        }
        out.write("]}", 2);
        out.done();
        return true;
      });
}
