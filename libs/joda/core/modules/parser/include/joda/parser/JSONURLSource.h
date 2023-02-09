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
class JSONURLSource : public joda::docparsing::IImportSource {
 public:
  explicit JSONURLSource(std::string urlPath, double sample = 1.0);

  virtual std::unique_ptr<queryexecution::pipeline::tasks::PipelineTask>
  getTask() const override;

  size_t estimatedSize() override;
  const std::string toString() override;
  const std::string toQueryString() override;

 private:
  std::string urlPath;
  double sample;
};
}  // namespace joda::docparsing

#endif  // JODA_JSONURLSOURCE_H
