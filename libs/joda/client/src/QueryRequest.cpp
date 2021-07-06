//
// Created by Nico on 23/08/2019.
//

#include "QueryRequest.h"
#include <glog/logging.h>
#include <joda/JodaClient.h>
#include <iostream>

std::pair<int64_t, int64_t> joda::network::client::QueryRequest::query(
    httplib::Client& client, const std::string& query,
    const std::string& prefix) {
  httplib::Params params;
  params.emplace("query", query);
  auto res = client.Post((prefix + endpoint).c_str(), params);

  if (res == nullptr) {
    throw(joda::network::client::JodaClientNoResponseException());
  }
  if (res->status != 200) {
    throw(joda::network::client::JodaServerHTTPException(res->status));
  }

  auto response = joda::network::client::JodaClient::parseResponse(*res);
  joda::network::client::JodaClient::checkError(response);

  if (!response.HasMember("success")) {
    throw(joda::network::client::JodaServerAPIErrorException(
        "Missing 'success' attribute in response"));
  }
  auto& success_ = response["success"];
  if (!success_.IsNumber()) {
    throw(joda::network::client::JodaServerAPIErrorException(
        "Invalid type of 'success' attribute in response"));
  }
  auto success = success_.GetInt64();

  if (!response.HasMember("size")) {
    throw(joda::network::client::JodaServerAPIErrorException(
        "Missing 'size' attribute in response"));
  }
  auto& size_ = response["size"];
  if (!size_.IsNumber()) {
    throw(joda::network::client::JodaServerAPIErrorException(
        "Invalid type of 'size' attribute in response"));
  }
  auto size = size_.GetInt64();

  if (size < 0) {
    if (!response.HasMember("message")) {
      throw(joda::network::client::JodaServerAPIErrorException(
          "Missing 'message' attribute in response"));
    }
    auto& message_ = response["message"];
    if (!message_.IsString()) {
      throw(joda::network::client::JodaServerAPIErrorException(
          "Invalid type of 'message' attribute in response"));
    }
    auto message = message_.GetString();
    std::cout << message << std::endl;
    return {0, 0};
  }
  if (size == 0) {
    std::cout << "Got empty result set" << std::endl;
    return {0, 0};
  } else {
    return {success, size};
  }
}
