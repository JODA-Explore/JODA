//
// Created by Nico on 30/11/2018.
//

#ifndef JODA_IIMPORTSOURCE_H
#define JODA_IIMPORTSOURCE_H

#include <joda/parser/ReaderFlags.h>
#include <joda/parser/ParserFlags.h>
#include <joda/container/ContainerFlags.h>

namespace joda::docparsing {
/**
 * Interface representing a list of documents to be imported into the system.
 */
class IImportSource {

 public:
 /**
  * Initializes a new IImportSource with the given flags
  * @param readerFlag the flags required by the reader to be able to read the documents
  * @param parserFlag the flags required by the parser to be able to parse the documents
  * @param containerFlag the flags required by the container to be able to store the documents
  */
  IImportSource(ReaderFlag readerFlag, ParserFlag parserFlag, ContainerFlag containerFlag)
      : readerFlag(readerFlag), parserFlag(parserFlag), containerFlag(containerFlag) {}

  virtual ~IImportSource() = default;

  ReaderFlag getReaderFlag() const {
    return readerFlag;
  }

  ParserFlag getParserFlag() const {
    return parserFlag;
  }

  ContainerFlag getContainerFlag() const {
    return containerFlag;
  }

  virtual void feedSources(JODA_READER_QUEUE<JODA_JSON_FILE_LINESEPERATED_READER_FLAG>::queue_t &queue,
                             JODA_READER_QUEUE<JODA_JSON_FILE_LINESEPERATED_READER_FLAG>::queue_t::ptok_t &ptok) {
    DCHECK(false) << "Called unimplemented queue JODA_JSON_FILE_LINESEPERATED_READER_FLAG";
  };

  virtual void feedSources(JODA_READER_QUEUE<JODA_JSON_FILE_BEAUTIFIED_READER_FLAG>::queue_t &queue,
                             JODA_READER_QUEUE<JODA_JSON_FILE_BEAUTIFIED_READER_FLAG>::queue_t::ptok_t &ptok) {
    DCHECK(false) << "Called unimplemented queue JODA_JSON_FILE_BEAUTIFIED_READER_FLAG";
  };

  virtual void feedSources(JODA_READER_QUEUE<JODA_JSON_URL_READER_FLAG>::queue_t &queue,
                           JODA_READER_QUEUE<JODA_JSON_URL_READER_FLAG>::queue_t::ptok_t &ptok) {
    DCHECK(false) << "Called unimplemented queue JODA_JSON_URL_BEAUTIFIED_READER_FLAG";
  };

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

 protected:
 public:

 private:
  ReaderFlag readerFlag;
  ParserFlag parserFlag;
  ContainerFlag containerFlag;
};
}

#endif //JODA_IIMPORTSOURCE_H
