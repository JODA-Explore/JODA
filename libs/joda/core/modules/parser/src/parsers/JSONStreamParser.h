//
// Created by Nico on 09/05/2019.
//

#ifndef JODA_JSONSTREAMPARSER_H
#define JODA_JSONSTREAMPARSER_H

#include <joda/concurrency/IOThreadPool.h>
#include <joda/parser/ParserFlags.h>
#include <joda/container/ContainerFlags.h>
#include <joda/container/JSONContainer.h>
#include <joda/config/config.h>
#include "ContainerScheduler/DefaultContainerScheduler.h"

namespace joda::docparsing {
/**
 * A parser which takes streams containing one or more documents.
 * The stream is read and each document is parsed until the stream ends.
 * @tparam Scheduler The document scheduler to use for clustering documents into containers
 */
template<class Scheduler = DefaultContainerScheduler<false>>
class JSONStreamParser : public IWorkerThread<JsonTextStreamParserQueue, JsonContainerQueue, size_t> {
 public:
  JSONStreamParser(IQueue *iqueue, OQueue *oqueue, size_t sourceSize) : IWorkerThread(iqueue, oqueue, sourceSize),
                                                                        sourceSize(this->conf) {
    oqueue->registerProducer();
    DLOG(INFO) << "Started JSONStreamParser";
  };

  ~JSONStreamParser() override {
    oqueue->unregisterProducer();
    DLOG(INFO) << "Stopped JSONTextParser";
  };

  static const size_t recommendedThreads() {
    return config::storageRetrievalThreads;
  };
 protected:
  void work() override {
    auto tok = IQueue::ctok_t(iqueue->queue);
    auto ptok = OQueue::ptok_t(oqueue->queue);

    Scheduler sched(oqueue, sourceSize);
    while (shouldRun) {
      if (!iqueue->isFinished()) {
        IPayload stream;
        iqueue->retrieve(stream);
        if (stream.first == nullptr) {
          LOG(WARNING) << "Got empty stream origin";
          continue;
        }
        if (stream.second == nullptr) {
          LOG(WARNING) << "Got empty stream";
          continue;
        }

        size_t index = 0;
        size_t begin, end = 0;
        if (*stream.second) {
          begin = stream.second->tellg();
          end = begin;
        } else {
          LOG(WARNING) << "Stream closed prematurely";
          continue;
        }

        rapidjson::IStreamWrapper isw(*stream.second);
        while (*stream.second) {
          std::shared_ptr<RJDocument> doc;
          typename Scheduler::ContainerIdentifier cont;
          if (std::is_same<Scheduler, joda::docparsing::DefaultContainerScheduler<false>>::value
              || std::is_same<Scheduler, joda::docparsing::DefaultContainerScheduler<true>>::value) {
            doc = sched.getNewDoc(0);
            doc->ParseStream<rapidjson::kParseStopWhenDoneFlag>(isw);
            if (doc->HasParseError()) {
              if (doc->GetParseError() != rapidjson::kParseErrorDocumentEmpty)
              LOG(WARNING) << std::string(rapidjson::GetParseError_En(doc->GetParseError())) <<
                           " from stream " << stream.first->getStreamName() << " : " << begin << "-"
                           << stream.second->tellg();

              continue;
            }
          } else {
            auto tmpDoc = std::make_shared<RJDocument>();
            tmpDoc->ParseStream<rapidjson::kParseStopWhenDoneFlag>(isw);
            if (doc->HasParseError()) {
              if (doc->GetParseError() != rapidjson::kParseErrorDocumentEmpty)
              LOG(WARNING) << std::string(rapidjson::GetParseError_En(doc->GetParseError())) <<
                           " from stream " << stream.first->getStreamName() << " : " << begin << "-"
                           << stream.second->tellg();

              continue;
            }
            cont = sched.getContainerForDoc(*tmpDoc);
            doc = sched.getNewDoc(cont);
            doc->CopyFrom(*tmpDoc, doc->GetAllocator(), true);
          }

          end = stream.second->tellg();

          auto ptr = stream.first->clone();
          auto *tmp = dynamic_cast<IDPositionOrigin *>(ptr.get());
          std::unique_ptr<IDPositionOrigin> orig;
          if (tmp != nullptr) {
            ptr.release();
            orig.reset(tmp);
          }
          if (orig == nullptr) {
            LOG(ERROR) << "Unsupported origin retrieved.";
            continue;
          }
          orig->setStart(begin);
          orig->setEnd(end);
          orig->setIndex(index);
          index++;

          sched.scheduleDocument(cont, std::move(doc), std::move(orig), end - begin);
          begin = end + 1;

        }
        if (stream.second->bad()) {
          LOG(ERROR) << "Error streaming: " << strerror(errno);
        }

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

#endif //JODA_JSONSTREAMPARSER_H
