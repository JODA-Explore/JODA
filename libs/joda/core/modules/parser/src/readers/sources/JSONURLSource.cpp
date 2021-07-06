//
// Created by Nico on 09/05/2019.
//

#include <utility>

#include "joda/parser/JSONURLSource.h"

joda::docparsing::JSONURLSource::JSONURLSource(std::string urlPath)
    : joda::docparsing::IImportSource(JODA_JSON_URL_READER_FLAG,
                                      JODA_JSON_TEXT_STREAM_PARSER_FLAG,
                                      JODA_JSON_CONTAINER_FLAG),
      urlPath(std::move(urlPath)) {}

void joda::docparsing::JSONURLSource::feedSources(
    JsonURLReaderQueue::queue_t& queue,
    JsonURLReaderQueue::queue_t::ptok_t& ptok) {
  URLPayload url{urlPath};
  queue.send(ptok, std::move(url));
}

size_t joda::docparsing::JSONURLSource::estimatedSize() {
  return IImportSource::estimatedSize();
}

const std::string joda::docparsing::JSONURLSource::toString() {
  return "URL: " + urlPath;
}

const std::string joda::docparsing::JSONURLSource::toQueryString() {
  std::string sampleString;

  return "FROM URL \"" + urlPath + "\"" + sampleString;
}
