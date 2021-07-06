//
// Created by Nico on 30/11/2018.
//

#include "../../../include/joda/parser/JSONFileDirectorySource.h"

#include <joda/fs/DirectoryFileScanner.h>

#include <utility>

joda::docparsing::JSONFileDirectorySource::JSONFileDirectorySource(
    const std::string& dirPath, const double sample)
    : joda::docparsing::JSONFileDirectorySource(dirPath, false, sample) {}

void joda::docparsing::JSONFileDirectorySource::feedSources(
    JODA_READER_QUEUE<JODA_JSON_FILE_LINESEPERATED_READER_FLAG>::queue_t& queue,
    JODA_READER_QUEUE<
        JODA_JSON_FILE_LINESEPERATED_READER_FLAG>::queue_t::ptok_t& ptok) {
  DCHECK(JODA_JSON_FILE_LINESEPERATED_READER_FLAG == getReaderFlag())
      << "Wrong flag called, this should be checked beforehand";
  joda::filesystem::DirectoryFileScanner scanner;
  auto files = scanner.getFilesInDir(dirPath, "json");
  for (auto&& item : files) {
    SampleFile file{sample, item};
    queue.send(ptok, std::move(file));
  }
  // TODO: Use chunks
  // queue.send(ptok,std::make_move_iterator(files.begin()), files.size());
}

void joda::docparsing::JSONFileDirectorySource::feedSources(
    JODA_READER_QUEUE<JODA_JSON_FILE_BEAUTIFIED_READER_FLAG>::queue_t& queue,
    JODA_READER_QUEUE<JODA_JSON_FILE_BEAUTIFIED_READER_FLAG>::queue_t::ptok_t&
        ptok) {
  DCHECK(JODA_JSON_FILE_BEAUTIFIED_READER_FLAG == getReaderFlag())
      << "Wrong flag called, this should be checked beforehand";
  joda::filesystem::DirectoryFileScanner scanner;
  auto files = scanner.getFilesInDir(dirPath, "json");
  for (auto&& item : files) {
    SampleFile file{sample, std::move(item)};
    queue.send(ptok, std::move(file));
  }
  // TODO: Use chunks
  // queue.send(ptok,std::make_move_iterator(files.begin()), files.size());
}

const std::string joda::docparsing::JSONFileDirectorySource::toString() {
  return "Directory: " + dirPath +
         "; Separated: " + std::to_string(static_cast<int>(lineSeparated)) +
         "; Sample: " + std::to_string(sample);
}

const std::string joda::docparsing::JSONFileDirectorySource::toQueryString() {
  std::string sampleString;
  if (sample < 1) {
    sampleString = " SAMPLE " + std::to_string(sample);
  }
  std::string separated;
  if (lineSeparated) {
    separated = " LINESEPARATED";
  }
  return "FROM FILES \"" + dirPath + "\"" + separated + sampleString;
}

joda::docparsing::JSONFileDirectorySource::JSONFileDirectorySource(
    std::string dirPath, bool lineSeparated, const double sample)
    : IImportSource(lineSeparated ? JODA_JSON_FILE_LINESEPERATED_READER_FLAG
                                  : JODA_JSON_FILE_BEAUTIFIED_READER_FLAG,
                    lineSeparated ? JODA_JSON_TEXT_PARSER_FLAG
                                  : JODA_JSON_TEXT_STREAM_PARSER_FLAG,
                    JODA_JSON_CONTAINER_FLAG),
      lineSeparated(lineSeparated),
      dirPath(std::move(dirPath)),
      sample(sample) {}

size_t joda::docparsing::JSONFileDirectorySource::estimatedSize() {
  size_t size = 0;
  joda::filesystem::DirectoryFileScanner dfs;
  for (auto&& item : dfs.getFilesInDir(dirPath, "json")) {
    size += dfs.getFileSize(item);
  }
  return size;
}
