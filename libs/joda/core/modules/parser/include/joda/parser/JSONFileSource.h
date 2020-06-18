//
// Created by Nico on 30/11/2018.
//

#ifndef JODA_JSONFILESOURCE_H
#define JODA_JSONFILESOURCE_H

#include "IImportSource.h"

namespace joda::docparsing {
/**
 * Represents a single JSON file to be imported
 */
class JSONFileSource : public joda::docparsing::IImportSource {
 public:
  JSONFileSource(const std::string &filePath, const double sample = 1);
  JSONFileSource(const std::string &filePath, bool lineSeparated, const double sample = 1);
  void feedSources(JODA_READER_QUEUE<JODA_JSON_FILE_LINESEPERATED_READER_FLAG>::queue_t &queue,
                     JODA_READER_QUEUE<JODA_JSON_FILE_LINESEPERATED_READER_FLAG>::queue_t::ptok_t &ptok) override;
  void feedSources(JODA_READER_QUEUE<JODA_JSON_FILE_BEAUTIFIED_READER_FLAG>::queue_t &queue,
                     JODA_READER_QUEUE<JODA_JSON_FILE_BEAUTIFIED_READER_FLAG>::queue_t::ptok_t &ptok) override;
  size_t estimatedSize() override;
  const std::string toString() override;
  const std::string toQueryString() override;
 private:
  std::string filePath;
  double sample;
  bool lineSeparated;
};
}

#endif //JODA_JSONFILESOURCE_H
