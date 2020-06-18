//
// Created by Nico on 27/11/2018.
//

#ifndef JODA_JSONTEXTPARSER_H
#define JODA_JSONTEXTPARSER_H

#include <joda/concurrency/IOThreadPool.h>
#include <joda/parser/ParserFlags.h>
#include <joda/container/ContainerFlags.h>
#include <joda/config/config.h>
#include <rapidjson/error/en.h>
#include <joda/storage/JSONStorage.h>
#include "ContainerScheduler/DefaultContainerScheduler.h"

namespace joda::docparsing {
/**
 * A parser which receives the textual representation of a document and parses it into the in-memory format.
 * @tparam Scheduler The document scheduler to use for clustering documents into containers
 */
template<class Scheduler = DefaultContainerScheduler<false>>
class JSONTextParser : public IWorkerThread<JsonTextParserQueue,
                                            JsonContainerQueue,
                                            size_t> {
 public:
  JSONTextParser(IQueue *iqueue, OQueue *oqueue, size_t sourceSize) : IWorkerThread(iqueue, oqueue, sourceSize),
                                                                      sourceSize(this->conf) {
    oqueue->registerProducer();
    DLOG(INFO) << "Started JSONTextParser";
  }

  ~JSONTextParser() override {
    oqueue->unregisterProducer();
    DLOG(INFO) << "Stopped JSONTextParser";
  };

  static const size_t recommendedThreads() {
    return config::storageRetrievalThreads;
  };
 protected:
  void work() override {
    auto tok = IQueue::ctok_t(iqueue->queue);
    //auto ptok = OQueue::ptok_t(oqueue->queue);
    std::vector<IPayload> buff;
    buff.reserve(config::parse_bulk_size);
    bool finishedItself = false;
    Scheduler sched(oqueue, sourceSize);
    while (shouldRun) {
      if (!iqueue->isFinished()) {
        DCHECK(buff.empty()) << "Buffer should be empty before receiving";
        auto count = iqueue->retrieve(tok, std::back_inserter(buff), buff.capacity());
        DCHECK_EQ(count, buff.size()) << "# of elements in buffer and # of returned elements should match";
        if (count == 0) continue;

        for (auto &text : buff) {
          if (text.first == nullptr) {
            LOG(WARNING) << "Got empty Origin for document";
            continue;
          }
          if (text.second.empty()) {
            LOG(WARNING) << "Got empty line for document";
            continue;
          }

          //Parse line
          auto cont = sched.getContainerForDoc(text.second);
          auto doc = sched.getNewDoc(cont);
          doc->Parse(text.second.c_str());
          if (doc->HasParseError()) {
            LOG(WARNING) << std::string(rapidjson::GetParseError_En(doc->GetParseError())) <<
                         " with Origin: " << text.first->toString();

            continue;
          }
          //Insert doc in container
          sched.scheduleDocument(cont,std::move(doc), std::move(text.first), text.second.size());
        }
        buff.clear();
      } else {
        shouldRun = false;
      }
    }
    sched.finalize();
  };

 private:
  size_t sourceSize;
};
}

#endif //JODA_JSONTEXTPARSER_H
