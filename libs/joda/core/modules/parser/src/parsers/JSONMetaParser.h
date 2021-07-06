//
// Created by Nico on 27/11/2018.
//

#ifndef JODA_JSONMETAPARSER_H
#define JODA_JSONMETAPARSER_H

#include <joda/concurrency/IOThreadPool.h>
#include <joda/container/ContainerFlags.h>
#include <joda/container/JSONContainer.h>
#include <joda/parser/ParserFlags.h>
#include "ContainerScheduler/DefaultContainerScheduler.h"

namespace joda::docparsing {
/**
 * A parser which does not actually parse the documents passed to it.
 * Instead, only the metadata of their origin required for actual parsing is
 * used. This parser is useful for parsing in memory-rescricted environments
 * @tparam Scheduler The document scheduler to use for clustering documents into
 * containers
 */
template <class Scheduler = DefaultContainerScheduler<true>>
class JSONMetaParser
    : public IWorkerThread<JsonTextParserQueue, JsonContainerQueue, size_t> {
 public:
  JSONMetaParser(IQueue *iqueue, OQueue *oqueue, size_t contSize)
      : IWorkerThread(iqueue, oqueue, contSize), contSize(this->conf) {
    oqueue->registerProducer();
    DLOG(INFO) << "Started JSONMetaParser";
  };

  ~JSONMetaParser() override {
    oqueue->unregisterProducer();
    DLOG(INFO) << "Stopped JSONMetaParser";
  };

 protected:
  void work() override {
    auto tok = IQueue::ctok_t(iqueue->queue);
    // auto ptok = OQueue::ptok_t(oqueue->queue);
    std::vector<IPayload> buff;
    buff.reserve(config::parse_bulk_size);
    Scheduler sched(oqueue, contSize);
    while (shouldRun) {
      if (!iqueue->isFinished()) {
        DCHECK(buff.empty()) << "Buffer should be empty before receiving";
        auto count =
            iqueue->retrieve(tok, std::back_inserter(buff), buff.capacity());
        DCHECK_EQ(count, buff.size()) << "# of elements in buffer and # of "
                                         "returned elements should match";
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

          // Insert doc in container
          auto cont = sched.getContainerForDoc(text.second);
          sched.scheduleDocument(cont, nullptr, std::move(text.first),
                                 text.second.size());
        }
        buff.clear();
      } else {
        shouldRun = false;
      }
    }
    sched.finalize();
  };

 private:
  size_t contSize;
};
}  // namespace joda::docparsing

#endif  // JODA_JSONMETAPARSER_H
