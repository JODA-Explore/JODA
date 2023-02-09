//
// Created by Nico on 09/05/2019.
//

#include "../../../include/joda/parser/JSONInStreamSource.h"

#include <joda/parser/pipeline/InStream.h>

#include <utility>

joda::docparsing::JSONInStreamSource::JSONInStreamSource(double sample)
    : sample(sample) {}

joda::docparsing::JSONInStreamSource::PipelineTaskPtr
joda::docparsing::JSONInStreamSource::getTask() const {
  return std::make_unique<
      queryexecution::pipeline::tasks::load::InStreamTask>(sample);
}

size_t joda::docparsing::JSONInStreamSource::estimatedSize() {
  return IImportSource::estimatedSize();
}

const std::string joda::docparsing::JSONInStreamSource::toString() {
  return "STREAM";
}

const std::string joda::docparsing::JSONInStreamSource::toQueryString() {
  std::string sampleString;
  if (sample < 1) {
    sampleString = " SAMPLE " + std::to_string(sample);
  }

  return "FROM STREAM" + sampleString;
}
