//
// Created by Nico on 30/11/2018.
//

#ifndef JODA_JSONFILEDIRECTORYSOURCE_H
#define JODA_JSONFILEDIRECTORYSOURCE_H

#include "IImportSource.h"

namespace joda::docparsing {

/**
 * Represents a directory containing one or multiple JSON files to be imported.
 */
class JSONFileDirectorySource : public joda::docparsing::IImportSource {
 public:
  explicit JSONFileDirectorySource(const std::string &dirPath, const double sample = 1);
  explicit JSONFileDirectorySource(const std::string &dirPath, bool lineSeparated, const double sample = 1);
  void feedSources(JODA_READER_QUEUE<JODA_JSON_FILE_LINESEPERATED_READER_FLAG>::queue_t &queue,
                     JODA_READER_QUEUE<JODA_JSON_FILE_LINESEPERATED_READER_FLAG>::queue_t::ptok_t &ptok) override;
  void feedSources(JODA_READER_QUEUE<JODA_JSON_FILE_BEAUTIFIED_READER_FLAG>::queue_t &queue,
                     JODA_READER_QUEUE<JODA_JSON_FILE_BEAUTIFIED_READER_FLAG>::queue_t::ptok_t &ptok) override;
  size_t estimatedSize() override;
  const std::string toString() override;
  const std::string toQueryString() override;
 private:
  std::string dirPath;
  double sample;
  bool lineSeparated;
};
}

#endif //JODA_JSONFILEDIRECTORYSOURCE_H
