//
// Created by Nico on 23/08/2019.
//

#include "ResultRequest.h"
#include <joda/JodaClient.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

std::vector<std::string> joda::network::client::ResultRequest::result(
    httplib::Client& client, const std::string& prefix, httplib::Params& params,
    bool pretty) {
  auto res = client.Post((prefix + endpoint).c_str(), params);

  if (res == nullptr) {
    throw(joda::network::client::JodaClientNoResponseException());
  }
  if (res->status != 200) {
    throw(joda::network::client::JodaServerHTTPException(res->status));
  }

  auto response = joda::network::client::JodaClient::parseResponse(*res);
  joda::network::client::JodaClient::checkError(response);

  if (!response.HasMember("result")) {
    throw(joda::network::client::JodaServerAPIErrorException(
        "Missing 'result' attribute in response"));
  }
  auto& result_ = response["result"];
  if (!result_.IsArray()) {
    throw(joda::network::client::JodaServerAPIErrorException(
        "Invalid type of 'result' attribute in response"));
  }

  std::vector<std::string> ret;

  if (pretty) {
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    for (auto& v : result_.GetArray()) {
      v.Accept(writer);
      std::string string = buffer.GetString();
      ret.emplace_back(string);
      buffer.Clear();
      writer.Reset(buffer);
    }
  } else {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    for (auto& v : result_.GetArray()) {
      v.Accept(writer);
      std::string string = buffer.GetString();
      ret.emplace_back(string);
      buffer.Clear();
      writer.Reset(buffer);
    }
  }
  return ret;
}

std::vector<std::string> joda::network::client::ResultRequest::result(
    httplib::Client& client, const std::string& prefix, unsigned int id,
    bool pretty) {
  httplib::Params params;
  params.emplace("id", std::to_string(id));
  return result(client, prefix, params, pretty);
}

std::vector<std::string> joda::network::client::ResultRequest::result(
    httplib::Client& client, const std::string& prefix, unsigned int id,
    unsigned int offset, unsigned int count, bool pretty) {
  httplib::Params params;
  params.emplace("id", std::to_string(id));
  params.emplace("offset", std::to_string(offset));
  params.emplace("count", std::to_string(count));
  return result(client, prefix, params, pretty);
}
