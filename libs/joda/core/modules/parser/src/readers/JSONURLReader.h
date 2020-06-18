//
// Created by Nico on 27/11/2018.
//

#ifndef JODA_JSONURLREADER_H
#define JODA_JSONURLREADER_H

#include <joda/parser/ReaderFlags.h>
#include <joda/parser/ParserFlags.h>
#include <joda/concurrency/IOThreadPool.h>
#include <joda/config/config.h>

namespace joda::docparsing {

/**
 * Opens the connection to a web-residing document and passes the input-strem to a parser
 */
class JSONURLReader : public IWorkerThread<JsonURLReaderQueue, JsonTextStreamParserQueue, void*>  {
 public:
  JSONURLReader(IQueue *iqueue, OQueue *oqueue, void* ptr);
  ~JSONURLReader() override;

  static const size_t recommendedThreads()   {
    return config::readingThreads;
  }
 protected:
  void work() override;


};
}

#endif //JODA_JSONURLREADER_H
