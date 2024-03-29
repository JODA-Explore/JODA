//
// Created by Nico on 30/11/2018.
//

#ifndef JODA_JSONFILESOURCE_H
#define JODA_JSONFILESOURCE_H

#include "IImportSource.h"

namespace joda::docparsing {
/**
 * Represents a single JSON file to be imported
 */
class JSONFileSource : public joda::docparsing::IImportSource {
 public:
  JSONFileSource(const std::string &filePath, const double sample = 1);

  JSONFileSource(std::string filePath, bool lineSeparated,
                 const double sample = 1.0);

  virtual std::unique_ptr<queryexecution::pipeline::tasks::PipelineTask>
  getTask() const override;

  size_t estimatedSize() override;

  const std::string toString() override;

  const std::string toQueryString() override;

 private:
  std::string filePath;
  double sample;
  bool lineSeparated;
};
}  // namespace joda::docparsing

#endif  // JODA_JSONFILESOURCE_H
