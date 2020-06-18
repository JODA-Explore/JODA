//
// Created by Nico on 16/05/2019.
//

#ifndef JODA_RJATTRIBUTEREADER_H
#define JODA_RJATTRIBUTEREADER_H

#include <string>
#include <vector>
#include <rapidjson/reader.h>
#include <set>

class RJAttributeReader : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, RJAttributeReader> {
 public:
  RJAttributeReader();
  bool Default();
  bool Null();
  bool Bool(bool b);
  bool Int(int i);
  bool Uint(unsigned int i);
  bool Int64(int64_t int64);
  bool Uint64(uint64_t uint64);
  bool Double(double d);
  bool RawNumber(const Ch *str, rapidjson::SizeType len, bool copy);
  bool String(const Ch *ch, rapidjson::SizeType type, bool b);
  bool StartObject();
  bool Key(const Ch *str, rapidjson::SizeType len, bool copy);
  bool EndObject(rapidjson::SizeType type);
  bool StartArray();
  bool EndArray(rapidjson::SizeType type);
  const std::set<std::string> &getAttributes() const;
  void clear();

 private:

  std::set<std::string> attributes;

};

#endif //JODA_RJATTRIBUTEREADER_H
