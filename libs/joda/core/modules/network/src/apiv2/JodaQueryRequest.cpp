//
// Created by Nico on 22/08/2019.
//
#include "JodaQueryRequest.h"

#include <joda/misc/Timer.h>
#include <joda/network/apiv2/API.h>
#include <joda/queryexecution/QueryPlan.h>
#include <joda/queryparsing/QueryParser.h>
#include <joda/storage/collection/StorageCollection.h>
#include "joda/network/JodaServer.h"

void joda::network::apiv2::JodaQueryRequest::registerEndpoint(
    const std::string& prefix, httplib::Server& server) {
  auto fullEndpoint = prefix + endpoint;
  server.Get(fullEndpoint.c_str(), [fullEndpoint](const httplib::Request& req,
                                                  httplib::Response& res) {
    try {
      JodaServer::logExecutionTime(fullEndpoint, [&]() { query(req, res); });
    } catch (JodaAPIException& e) {
      JodaServer::handleError(e, res);
    }
  });

  server.Post(fullEndpoint.c_str(), [fullEndpoint](const httplib::Request& req,
                                                   httplib::Response& res) {
    try {
      JodaServer::logExecutionTime(fullEndpoint, [&]() { query(req, res); });
    } catch (JodaAPIException& e) {
      JodaServer::handleError(e, res);
    }
  });
}

std::vector<std::string> joda::network::apiv2::JodaQueryRequest::splitQueries(
    const std::string& query) {
  // https://thispointer.com/how-to-split-a-string-in-c/
  std::stringstream ss(query);
  std::string item;
  std::vector<std::string> splittedStrings;
  while (std::getline(ss, item, ';')) {
    splittedStrings.push_back(item);
  }
  return splittedStrings;
}

void joda::network::apiv2::JodaQueryRequest::query(const httplib::Request& req,
                                                   httplib::Response& res) {
  auto q = req.params.find("query");
  if (q == req.params.end()) {
    throw(JodaQueryException());
  }
  LOG(INFO) << "Got query: " << q->second;

  auto querystrings = splitQueries(q->second);

  LOG(INFO) << "Got " << querystrings.size() << " queries.";

  queryparsing::QueryParser parser;
  std::vector<std::shared_ptr<query::Query>> queries;
  for (const auto& q : querystrings) {
    auto query = parser.parse(q);
    if (query == nullptr) {
      throw(JodaQueryException(parser.getLastError()));
    }
    queries.push_back(std::move(query));
  }

  Benchmark bench;
  unsigned long result;
  for (const auto& query : queries) {
    QueryPlan plan(query);
    bench.addValue("Query", query->toString());
    LOG(INFO) << "Executing query:\n" << query->toString();
    Timer timer;
    result = plan.executeQuery(&bench);
    timer.stop();
    bench.addValue(Benchmark::RUNTIME, "Query", timer.durationSeconds());
    bench.finishLine();
  }
  if (result < JODA_STORE_VALID_ID_START) {
    if (result == JODA_STORE_EXTERNAL_RS_ID) {
      return export_(res);
    }
    if (result == JODA_STORE_EMPTY_RS_ID) {
      return empty_(res);
    } else if (result == JODA_STORE_SKIPPED_QUERY_ID) {
      return skipped_(res);
    } else {
      return error_(result, res);
    }
  } else {
    return success_(result, res, bench);
  }
}

void joda::network::apiv2::JodaQueryRequest::error_(
    unsigned long code, httplib::Response& /*res*/) {
  LOG(ERROR) << "Got invalid unknown special code as query result: " << code;
  throw(JodaQueryUnkownErrorException());
}

void joda::network::apiv2::JodaQueryRequest::skipped_(
    httplib::Response& /*res*/) {
  LOG(WARNING) << "Query got skipped (" << JODA_STORE_SKIPPED_QUERY_ID << ")";
  throw(JodaQuerySkippedException());
}

void joda::network::apiv2::JodaQueryRequest::empty_(httplib::Response& res) {
  auto doc = successDocument(JODA_STORE_EMPTY_RS_ID, -1,
                             "The query was executed on an empty collection.");
  JodaServer::sendResponse(doc, res);
}

RJDocument joda::network::apiv2::JodaQueryRequest::successDocument(
    unsigned long result, int64_t size, const std::string& message,
    const Benchmark* benchmark) {
  RJDocument r(rapidjson::kObjectType);
  RJValue _success(result);
  RJValue _size(size);
  RJValue _message(message, r.GetAllocator());

  r.AddMember("success", _success, r.GetAllocator());
  r.AddMember("size", _size, r.GetAllocator());
  if (!message.empty()) {
    r.AddMember("message", message, r.GetAllocator());
  }
  if (benchmark != nullptr && benchmark->isValid()) {
    RJValue benchVal;
    benchVal.CopyFrom(benchmark->getLastLine(), r.GetAllocator());
    r.AddMember("benchmark", benchVal, r.GetAllocator());
  }

  return r;
}

void joda::network::apiv2::JodaQueryRequest::success_(
    unsigned long result, httplib::Response& res, const Benchmark& benchmark) {
  auto r = StorageCollection::getInstance().getStorage(result);
  if (r == nullptr) {
    LOG(ERROR) << "Could not find collection which was assumed valid.";
    throw(JodaQueryUnkownErrorException());
  }
  auto doc = successDocument(result, r->size(), "", &benchmark);
  DLOG(INFO) << "Successful query: " << result << " (size: " << r->size()
             << ")";
  JodaServer::sendResponse(doc, res);
}

void joda::network::apiv2::JodaQueryRequest::export_(httplib::Response& res) {
  auto doc = successDocument(JODA_STORE_EXTERNAL_RS_ID, -1,
                             "The query successfully exported the data.");
  JodaServer::sendResponse(doc, res);
}
