//
// Created by Nico on 30/11/2018.
//

#include "../../../include/joda/parser/JSONFileSource.h"
#include <joda/fs/DirectoryFileScanner.h>

#include <utility>

joda::docparsing::JSONFileSource::JSONFileSource(const std::string& filePath,
                                                 const double sample)
    : JSONFileSource(filePath, false, sample) {}

void joda::docparsing::JSONFileSource::feedSources(
    JODA_READER_QUEUE<JODA_JSON_FILE_LINESEPERATED_READER_FLAG>::queue_t& queue,
    JODA_READER_QUEUE<
        JODA_JSON_FILE_LINESEPERATED_READER_FLAG>::queue_t::ptok_t& ptok) {
  DCHECK(JODA_JSON_FILE_LINESEPERATED_READER_FLAG == getReaderFlag())
      << "Wrong flag called, this should be checked beforehand";

  // Send single file to queue
  SampleFile file{sample, filePath};
  queue.send(ptok, std::move(file));
}

void joda::docparsing::JSONFileSource::feedSources(
    JODA_READER_QUEUE<JODA_JSON_FILE_BEAUTIFIED_READER_FLAG>::queue_t& queue,
    JODA_READER_QUEUE<JODA_JSON_FILE_BEAUTIFIED_READER_FLAG>::queue_t::ptok_t&
        ptok) {
  DCHECK(JODA_JSON_FILE_BEAUTIFIED_READER_FLAG == getReaderFlag())
      << "Wrong flag called, this should be checked beforehand";
  // Send single file to queue
  SampleFile file{sample, filePath};
  queue.send(ptok, std::move(file));
}

const std::string joda::docparsing::JSONFileSource::toString() {
  return "File: " + filePath +
         "; Separated: " + std::to_string(static_cast<int>(lineSeparated)) +
         "; Sample: " + std::to_string(sample);
}

const std::string joda::docparsing::JSONFileSource::toQueryString() {
  std::string sampleString;
  if (sample < 1) {
    sampleString = " SAMPLE " + std::to_string(sample);
  }
  std::string separated;
  if (lineSeparated) {
    separated = " LINESEPARATED";
  }
  return "FROM FILE \"" + filePath + "\"" + separated + sampleString;
}

joda::docparsing::JSONFileSource::JSONFileSource(std::string filePath,
                                                 bool lineSeparated,
                                                 const double sample)
    : IImportSource(lineSeparated ? JODA_JSON_FILE_LINESEPERATED_READER_FLAG
                                  : JODA_JSON_FILE_BEAUTIFIED_READER_FLAG,
                    lineSeparated ? JODA_JSON_TEXT_PARSER_FLAG
                                  : JODA_JSON_TEXT_STREAM_PARSER_FLAG,
                    JODA_JSON_CONTAINER_FLAG),
      filePath(std::move(filePath)),
      sample(sample),
      lineSeparated(lineSeparated) {}

size_t joda::docparsing::JSONFileSource::estimatedSize() {
  joda::filesystem::DirectoryFileScanner dfs;
  return dfs.getFileSize(filePath)*config::text_binary_mod;
}
