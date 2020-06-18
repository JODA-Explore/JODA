//
// Created by Nico on 09/05/2019.
//

#ifndef JODA_JSONURLSOURCE_H
#define JODA_JSONURLSOURCE_H

#include "IImportSource.h"

namespace joda::docparsing {
/**
 * Represents a web url which contains a single JSON file to be imported.
 */
class JSONURLSource : public joda::docparsing::IImportSource{
 public:
  explicit JSONURLSource(const std::string &urlPath);
  virtual void feedSources(JsonURLReaderQueue::queue_t &queue,
                           JsonURLReaderQueue::queue_t::ptok_t &ptok);
  size_t estimatedSize() override;
  const std::string toString() override;
  const std::string toQueryString() override;
 private:
  std::string urlPath;
};
}

#endif //JODA_JSONURLSOURCE_H
