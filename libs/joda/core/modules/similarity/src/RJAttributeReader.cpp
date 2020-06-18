//
// Created by Nico on 11/09/2018.
//

#include <glog/logging.h>
#include "RJAttributeReader.h"

bool RJAttributeReader::Default() {
  return true;
}

bool RJAttributeReader::Null() {

  return true;
}

bool RJAttributeReader::Bool(bool b) {

  return true;
}

bool RJAttributeReader::Int(int i) {

  return true;
}

bool RJAttributeReader::Uint(unsigned int i) {

  return true;
}

bool RJAttributeReader::Int64(int64_t int64) {

  return true;
}

bool RJAttributeReader::Uint64(uint64_t uint64) {

  return true;
}

bool RJAttributeReader::Double(double d) {

  return true;
}

bool RJAttributeReader::RawNumber(const char *str, rapidjson::SizeType len, bool copy) {

  return true;
}

bool RJAttributeReader::String(const char *ch, rapidjson::SizeType type, bool b) {

  return true;
}

bool RJAttributeReader::StartObject() {

  return true;
}

bool RJAttributeReader::Key(const char *str, rapidjson::SizeType len, bool copy) {
  attributes.insert({str, len});
  return true;
}

bool RJAttributeReader::EndObject(rapidjson::SizeType type) {

  return true;
}

bool RJAttributeReader::StartArray() {

  return true;
}

bool RJAttributeReader::EndArray(rapidjson::SizeType type) {
  return true;
}

RJAttributeReader::RJAttributeReader() : attributes() {

}

const std::set<std::string> &RJAttributeReader::getAttributes() const {
  return attributes;
}

void RJAttributeReader::clear() {
  attributes.clear();
}

