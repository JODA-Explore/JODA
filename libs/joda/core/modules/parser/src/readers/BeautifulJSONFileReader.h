//
// Created by Nico on 05/12/2018.
//

#ifndef JODA_BEAUTIFULJSONFILEREADER_H
#define JODA_BEAUTIFULJSONFILEREADER_H

#include <joda/concurrency/IOThreadPool.h>
#include <joda/parser/ReaderFlags.h>
#include <joda/parser/ParserFlags.h>
#include <joda/config/config.h>

namespace joda::docparsing {
/**
 * Reads formatted (multi-line) JSON documents.
 * This requires streaming parser, as the whole syntax has to be checked
 */
class BeautifulJSONFileReader : public IWorkerThread<JsonFileBeautifiedReaderQueue, JsonTextStreamParserQueue, void *> {
 public:
  BeautifulJSONFileReader(IQueue *iqueue, OQueue *oqueue, void* ptr);
  ~BeautifulJSONFileReader() override;

  static const size_t recommendedThreads()   {
    return config::readingThreads;
  }
 protected:
  void work() override;


};
}

#endif //JODA_BEAUTIFULJSONFILEREADER_H
