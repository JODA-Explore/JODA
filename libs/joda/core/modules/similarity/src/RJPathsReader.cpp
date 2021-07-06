//
// Created by Nico on 11/09/2018.
//

#include "RJPathsReader.h"
#include <glog/logging.h>

bool RJPathsReader::Default() { return true; }

bool RJPathsReader::Null() {
  popStack();
  incArray();
  return true;
}

bool RJPathsReader::Bool(bool /*b*/) {
  popStack();
  incArray();
  return true;
}

bool RJPathsReader::Int(int /*i*/) {
  popStack();
  incArray();
  return true;
}

bool RJPathsReader::Uint(unsigned int /*i*/) {
  popStack();
  incArray();
  return true;
}

bool RJPathsReader::Int64(int64_t /*int64*/) {
  popStack();
  incArray();
  return true;
}

bool RJPathsReader::Uint64(uint64_t /*uint64*/) {
  popStack();
  incArray();
  return true;
}

bool RJPathsReader::Double(double /*d*/) {
  popStack();
  incArray();
  return true;
}

bool RJPathsReader::RawNumber(const char* /*str*/, rapidjson::SizeType /*len*/,
                              bool /*copy*/) {
  popStack();
  incArray();
  return true;
}

bool RJPathsReader::String(const char* /*ch*/, rapidjson::SizeType /*type*/,
                           bool /*b*/) {
  popStack();
  incArray();
  return true;
}

bool RJPathsReader::StartObject() {
  arrStack.emplace_back(-1);
  return true;
}

bool RJPathsReader::Key(const char* str, rapidjson::SizeType len,
                        bool /*copy*/) {
  addStack(str, len);
  addPath();
  return true;
}

bool RJPathsReader::EndObject(rapidjson::SizeType /*type*/) {
  popStack();
  arrStack.pop_back();
  incArray();
  return true;
}

bool RJPathsReader::StartArray() {
  addArray();
  return true;
}

bool RJPathsReader::EndArray(rapidjson::SizeType /*type*/) {
  stopArray();
  popStack();  // Pop index of array
  popStack();  // Pop Parent
  incArray();
  return true;
}

RJPathsReader::RJPathsReader() {
  stack.reserve(JODA_DEFAULT_PATH_DEPTH);
  paths.reserve(JODA_DEFAULT_PATH_DEPTH);
}

void RJPathsReader::addStack(const char* str, rapidjson::SizeType len) {
  stack.emplace_back(str, len);
}

void RJPathsReader::popStack() {
  if (stack.empty()) {
    return;
  }
  stack.pop_back();
}

const std::vector<std::string>& RJPathsReader::getPaths() const {
  return paths;
}

void RJPathsReader::addArray() {
  arrStack.emplace_back(0);
  auto string = std::to_string(arrStack.back());
  addStack(string.c_str(), string.size());
}

void RJPathsReader::incArray() {
  if (!arrStack.empty() && arrStack.back() >= 0) {
    arrStack.back()++;
    auto string = std::to_string(arrStack.back());
    addStack(string.c_str(), string.size());
  }
}

void RJPathsReader::stopArray() { arrStack.pop_back(); }

void RJPathsReader::clear() {
  stack.clear();
  arrStack.clear();
  paths.clear();
}

void RJPathsReader::addPath() {
  std::string stackstr;
  for (const auto& item : stack) {
    stackstr += "/" + item;
  }
  paths.emplace_back(std::move(stackstr));
}
