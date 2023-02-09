//
// Created by Nico on 30/11/2018.
//

#ifndef JODA_IIMPORTSOURCE_H
#define JODA_IIMPORTSOURCE_H

#include <joda/container/ContainerFlags.h>
#include <joda/parser/ParserFlags.h>
#include <joda/parser/ReaderFlags.h>
#include <joda/pipelineatomics/PipelineTask.h>

namespace joda::docparsing {
/**
 * Interface representing a list of documents to be imported into the system.
 */
class IImportSource {
 protected:
    typedef joda::queryexecution::pipeline::tasks::PipelineTaskPtr PipelineTaskPtr;
 public:

  IImportSource() = default;

  virtual ~IImportSource() = default;

  /**
   * Returns the loading task
   * @return the task to be executed to load the documents
   */
  virtual PipelineTaskPtr getTask() const = 0;


  /**
   * Returns a (human readable) string describing the IImportSource.
   * A.toString() == B.toString() => A == B
   * @return
   */
  virtual const std::string toString() = 0;

  /**
   * Returns a string as this IImportSource would be represented in a query
   * @return
   */
  virtual const std::string toQueryString() = 0;

  /**
   * Estimates the size of the document to be imported
   */
  virtual size_t estimatedSize() { return 0; }

};
}  // namespace joda::docparsing

#endif  // JODA_IIMPORTSOURCE_H
