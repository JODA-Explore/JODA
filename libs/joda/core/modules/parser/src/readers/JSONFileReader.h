//
// Created by Nico on 27/11/2018.
//

#ifndef JODA_JSONFILEREADER_H
#define JODA_JSONFILEREADER_H

#include <joda/concurrency/IOThreadPool.h>
#include <joda/config/config.h>
#include <joda/parser/ParserFlags.h>
#include <joda/parser/ReaderFlags.h>

namespace joda::docparsing {
/**
 * Reads a file containing line-seperated JSON files.
 * This is more efficient, as it only needs to scan for a single character per
 * document.
 */
class JSONFileReader : public IWorkerThread<JsonFileSeperatedReaderQueue,
                                            JsonTextParserQueue, void *> {
 public:
  JSONFileReader(IQueue *iqueue, OQueue *oqueue, void *ptr);
  ~JSONFileReader() override;

  static const size_t recommendedThreads() { return config::readingThreads; }

 protected:
  void work() override;
};
}  // namespace joda::docparsing

#endif  // JODA_JSONFILEREADER_H
