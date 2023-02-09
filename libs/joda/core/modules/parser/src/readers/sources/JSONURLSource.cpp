//
// Created by Nico on 09/05/2019.
//

#include <utility>

#include "joda/parser/JSONURLSource.h"
#include <joda/parser/pipeline/URLStream.h>

joda::docparsing::JSONURLSource::JSONURLSource(std::string urlPath, double sample)
    :       urlPath(std::move(urlPath)), sample(sample) {}

joda::docparsing::JSONURLSource::PipelineTaskPtr
joda::docparsing::JSONURLSource::getTask() const {
  return std::make_unique<queryexecution::pipeline::tasks::load::URLStreamTask>(urlPath, sample);
}

size_t joda::docparsing::JSONURLSource::estimatedSize() {
  return IImportSource::estimatedSize();
}

const std::string joda::docparsing::JSONURLSource::toString() {
  return "URL: " + urlPath;
}

const std::string joda::docparsing::JSONURLSource::toQueryString() {
  std::string sampleString;
  if (sample < 1) {
    sampleString = " SAMPLE " + std::to_string(sample);
  }
  return "FROM URL \"" + urlPath + "\"" + sampleString;
}
