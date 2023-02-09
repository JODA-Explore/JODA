//
// Created by Nico on 30/11/2018.
//

#ifndef JODA_JSONFILEDIRECTORYSOURCE_H
#define JODA_JSONFILEDIRECTORYSOURCE_H

#include "IImportSource.h"

namespace joda::docparsing {

/**
 * Represents a directory containing one or multiple JSON files to be imported.
 */
class JSONFileDirectorySource : public joda::docparsing::IImportSource {
 public:
  explicit JSONFileDirectorySource(const std::string &dirPath,
                                   const double sample = 1);
  explicit JSONFileDirectorySource(std::string dirPath, bool lineSeparated,
                                   const double sample = 1.0);
  ~JSONFileDirectorySource() override = default;

  virtual PipelineTaskPtr getTask() const override;

  size_t estimatedSize() override;

  const std::string toString() override;

  const std::string toQueryString() override;

 private:
  bool lineSeparated;
  std::string dirPath;
  double sample;
};
}  // namespace joda::docparsing

#endif  // JODA_JSONFILEDIRECTORYSOURCE_H
