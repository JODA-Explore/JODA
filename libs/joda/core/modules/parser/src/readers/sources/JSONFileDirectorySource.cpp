//
// Created by Nico on 30/11/2018.
//

#include "../../../include/joda/parser/JSONFileDirectorySource.h"
#include <joda/parser/pipeline/ListDir.h>

#include <joda/fs/DirectoryFileScanner.h>
#include <utility>

joda::docparsing::JSONFileDirectorySource::JSONFileDirectorySource(
    const std::string& dirPath, const double sample)
    : joda::docparsing::JSONFileDirectorySource(dirPath, false, sample) {}

joda::docparsing::JSONFileDirectorySource::PipelineTaskPtr joda::docparsing::JSONFileDirectorySource::getTask() const {
  if(lineSeparated){
    return std::make_unique<queryexecution::pipeline::tasks::load::LSListDirTask>(dirPath, sample);
  }
  return std::make_unique<queryexecution::pipeline::tasks::load::ListDirTask>(dirPath,sample);
  
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
    std::string dirPath, bool lineSeparated, const double sample) :
      lineSeparated(lineSeparated),
      dirPath(std::move(dirPath)),
      sample(sample) {}

size_t joda::docparsing::JSONFileDirectorySource::estimatedSize() {
  size_t size = 0;
  joda::filesystem::DirectoryFileScanner dfs;
  for (auto&& item : dfs.getFilesInDir(dirPath, "json")) {
    size += dfs.getFileSize(item);
  }
  return size * config::text_binary_mod;
}
