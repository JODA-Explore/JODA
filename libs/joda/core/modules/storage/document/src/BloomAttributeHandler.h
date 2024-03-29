//
// Created by Nico on 11/09/2018.
//

#ifndef JODA_RJPATHSREADER_H
#define JODA_RJPATHSREADER_H

#include <rapidjson/reader.h>
#include <string>
#include <vector>
#define JODA_DEFAULT_PATH_DEPTH 50

class BloomAttributeHandler
    : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>,
                                          BloomAttributeHandler> {
 public:
  BloomAttributeHandler();
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
  const std::vector<std::string> &getPaths() const;
  void clear();

 private:
  std::vector<std::string> stack;
  std::vector<std::string> paths;
  std::vector<long> arrStack;
  void addPath();
  void addArray();
  void incArray();
  void stopArray();
  void addStack(const Ch *str, rapidjson::SizeType len);
  void popStack();
};

#endif  // JODA_RJPATHSREADER_H
