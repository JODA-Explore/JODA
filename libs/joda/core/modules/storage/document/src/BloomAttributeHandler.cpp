//
// Created by Nico on 11/09/2018.
//

#include "BloomAttributeHandler.h"
#include <glog/logging.h>

bool BloomAttributeHandler::Default() { return true; }

bool BloomAttributeHandler::Null() {
  popStack();
  incArray();
  return true;
}

bool BloomAttributeHandler::Bool(bool /*b*/) {
  popStack();
  incArray();
  return true;
}

bool BloomAttributeHandler::Int(int /*i*/) {
  popStack();
  incArray();
  return true;
}

bool BloomAttributeHandler::Uint(unsigned int /*i*/) {
  popStack();
  incArray();
  return true;
}

bool BloomAttributeHandler::Int64(int64_t /*int64*/) {
  popStack();
  incArray();
  return true;
}

bool BloomAttributeHandler::Uint64(uint64_t /*uint64*/) {
  popStack();
  incArray();
  return true;
}

bool BloomAttributeHandler::Double(double /*d*/) {
  popStack();
  incArray();
  return true;
}

bool BloomAttributeHandler::RawNumber(const char* /*str*/,
                                      rapidjson::SizeType /*len*/,
                                      bool /*copy*/) {
  popStack();
  incArray();
  return true;
}

bool BloomAttributeHandler::String(const char* /*ch*/,
                                   rapidjson::SizeType /*type*/, bool /*b*/) {
  popStack();
  incArray();
  return true;
}

bool BloomAttributeHandler::StartObject() {
  arrStack.emplace_back(-1);
  return true;
}

bool BloomAttributeHandler::Key(const char* str, rapidjson::SizeType len,
                                bool /*copy*/) {
  addStack(str, len);
  addPath();
  return true;
}

bool BloomAttributeHandler::EndObject(rapidjson::SizeType /*type*/) {
  popStack();
  arrStack.pop_back();
  incArray();
  return true;
}

bool BloomAttributeHandler::StartArray() {
  addArray();
  return true;
}

bool BloomAttributeHandler::EndArray(rapidjson::SizeType /*type*/) {
  stopArray();
  popStack();  // Pop index of array
  popStack();  // Pop Parent
  incArray();
  return true;
}

BloomAttributeHandler::BloomAttributeHandler() {
  stack.reserve(JODA_DEFAULT_PATH_DEPTH);
  paths.reserve(JODA_DEFAULT_PATH_DEPTH);
}

void BloomAttributeHandler::addStack(const char* str, rapidjson::SizeType len) {
  stack.emplace_back(str, len);
}

void BloomAttributeHandler::popStack() {
  if (stack.empty()) {
    return;
  }
  stack.pop_back();
}

const std::vector<std::string>& BloomAttributeHandler::getPaths() const {
  return paths;
}

void BloomAttributeHandler::addArray() {
  arrStack.emplace_back(0);
  auto string = std::to_string(arrStack.back());
  addStack(string.c_str(), string.size());
}

void BloomAttributeHandler::incArray() {
  if (!arrStack.empty() && arrStack.back() >= 0) {
    arrStack.back()++;
    auto string = std::to_string(arrStack.back());
    addStack(string.c_str(), string.size());
  }
}

void BloomAttributeHandler::stopArray() { arrStack.pop_back(); }

void BloomAttributeHandler::clear() {
  stack.clear();
  arrStack.clear();
  paths.clear();
}

void BloomAttributeHandler::addPath() {
  std::string stackstr;
  if (!arrStack.empty()) {
    return;
  }
  for (const auto& item : stack) {
    stackstr += "/" + item;
  }
  paths.emplace_back(std::move(stackstr));
}
