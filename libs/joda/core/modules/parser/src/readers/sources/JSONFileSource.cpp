//
// Created by Nico on 30/11/2018.
//

#include "../../../include/joda/parser/JSONFileSource.h"

#include <joda/fs/DirectoryFileScanner.h>

#include <utility>

#include <joda/parser/pipeline/ListFile.h>

joda::docparsing::JSONFileSource::JSONFileSource(const std::string& filePath,
                                                 const double sample)
    : JSONFileSource(filePath, false, sample) {}

joda::docparsing::JSONFileSource::PipelineTaskPtr
joda::docparsing::JSONFileSource::getTask() const {
  if(lineSeparated){
    return std::make_unique<queryexecution::pipeline::tasks::load::LSListFileTask>(
      filePath, sample);
  }
  return std::make_unique<queryexecution::pipeline::tasks::load::ListFileTask>(
      filePath, sample);
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
    :       filePath(std::move(filePath)),
      sample(sample),
      lineSeparated(lineSeparated) {}

size_t joda::docparsing::JSONFileSource::estimatedSize() {
  joda::filesystem::DirectoryFileScanner dfs;
  return dfs.getFileSize(filePath) * config::text_binary_mod;
}
