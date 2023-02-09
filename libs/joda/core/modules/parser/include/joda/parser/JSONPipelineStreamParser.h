//
// Created by Nico on 27/11/2018.
//

#ifndef JODA_JSON_PIPELINE_STREAMPARSER_H
#define JODA_JSON_PIPELINE_STREAMPARSER_H

#include <joda/concurrency/IOThreadPool.h>
#include <joda/config/config.h>
#include <joda/container/ContainerFlags.h>
#include <joda/parser/ParserFlags.h>
#include <joda/parser/ParserIntermediates.h>
#include <joda/pipelineatomics/queue/DefaultPipelineQueues.h>
#include <joda/storage/JSONStorage.h>
#include <rapidjson/error/en.h>

#include <iostream>

#include "../src/parsers/ContainerScheduler/DefaultContainerScheduler.h"

namespace joda::docparsing {
/**
 * A parser which receives the textual representation of a document and parses
 * it into the in-memory format.
 * @tparam Scheduler The document scheduler to use for clustering documents into
 * containers
 */
template <class Scheduler = DefaultContainerScheduler<false>>
class JSONPipelineStreamParser {
 public:
  typedef queryexecution::pipeline::queue::InputQueueTrait<
      std::pair<StreamOrigin, double>>
      I;
  typedef queryexecution::pipeline::queue::OutputQueueTrait<
      std::shared_ptr<JSONContainer>>
      O;

  typedef I::Input Input;
  typedef O::Output Output;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1;

  JSONPipelineStreamParser(size_t sourceSize)
      : sourceSize(sourceSize), sched(sourceSize) {
    DLOG(INFO) << "Started JSONPipelineStreamParser";
  }

  JSONPipelineStreamParser(const JSONPipelineStreamParser& o)
      : sourceSize(o.sourceSize), sched(o.sourceSize) {
    DLOG(INFO) << "Started JSONPipelineStreamParser";
  }

  ~JSONPipelineStreamParser() {
    DLOG(INFO) << "Stopped JSONPipelineStreamParser";
  };

  void fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff,
                  std::function<void(std::optional<Output>&)> sendPartial) {
    if(ibuff.has_value()) {
      auto in = std::move(ibuff.value());
      ibuff.reset();
      auto& origin = in.first;
      auto sample = in.second;

      if (origin.stream == nullptr) {
        LOG(WARNING) << "Got empty stream";
        return;
      }
      if (origin.streamOrigin == nullptr) {
        LOG(WARNING) << "Got empty stream origin";
        return;
      }
      size_t index = 0;
      rapidjson::IStreamWrapper isw(*origin.stream);

      auto end = isw.Tell();
      rapidjson::MemoryPoolAllocator sample_alloc;
      while (true) {
        auto begin = isw.Tell();
        std::unique_ptr<RJDocument> doc;

        // If not sampled => parse into sample alloc and discard
        // This is needed to get the correct stream offsets
        if (sample < 1.0 && (static_cast<double>(rand() % 100)) / 100.0 >=
                                sample) {  // Sampling
          RJDocument sample_doc(&sample_alloc);
          sample_doc.ParseStream<rapidjson::kParseStopWhenDoneFlag>(isw);
          sample_alloc.Clear();
          continue;
        }

        typename Scheduler::ContainerIdentifier cont = {};
        if (std::is_same<Scheduler, joda::docparsing::DefaultContainerScheduler<
                                        false>>::value ||
            std::is_same<Scheduler, joda::docparsing::DefaultContainerScheduler<
                                        true>>::value) {
          doc = std::move(sched.getNewDoc(0));
          doc->ParseStream<rapidjson::kParseStopWhenDoneFlag>(isw);
          if (doc->HasParseError()) {
            if (doc->GetParseError() !=
                rapidjson::kParseErrorDocumentEmpty) {
              LOG(WARNING) << std::string(rapidjson::GetParseError_En(
                                  doc->GetParseError()))
                           << " from stream "
                           << origin.streamOrigin->getStreamName() << " : "
                           << begin << "-" << isw.Tell();
              continue;
            } else {
              break;
            }
          }
        } else {
          auto tmpDoc = std::make_unique<RJDocument>();
          tmpDoc->ParseStream<rapidjson::kParseStopWhenDoneFlag>(isw);
          if (tmpDoc->HasParseError()) {
            if (tmpDoc->GetParseError() !=
                rapidjson::kParseErrorDocumentEmpty) {
              LOG(WARNING) << std::string(rapidjson::GetParseError_En(
                                  tmpDoc->GetParseError()))
                           << " from stream "
                           << origin.streamOrigin->getStreamName() << " : "
                           << begin << "-" << isw.Tell();
              continue;
            } else {
              break;
            }
          }
          cont = sched.getContainerForDoc(*tmpDoc);
          doc = std::move(sched.getNewDoc(cont));
          doc->CopyFrom(*tmpDoc, doc->GetAllocator(), true);
        }

        end = isw.Tell();

        auto ptr = origin.streamOrigin->clone();
        auto* tmp = dynamic_cast<IDPositionOrigin*>(ptr.get());
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

        auto cont_ptr = sched.scheduleDocument(cont, std::move(doc),
                                               std::move(orig), end - begin);
        if (cont_ptr != nullptr) {
          obuff = std::move(cont_ptr);
          sendPartial(obuff);
          DCHECK(!obuff.has_value());
        }
      }
      if (origin.stream->bad()) {
        LOG(ERROR) << "Error streaming: " << strerror(errno);
      }
      if (origin.stream.get() == &std::cin) {  // Do not deconstruct cin
        origin.stream.release();
      }
    }
  }

  void finalize(std::optional<O::Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial) {
    auto ptrs = sched.finalize();
    for (auto& ptr : ptrs) {
      obuff = std::move(ptr);
      sendPartial(obuff);
    }
  }

 private:
  size_t sourceSize;
  Scheduler sched;
};
}  // namespace joda::docparsing

#endif  // JODA_JSON_PIPELINE_STREAMPARSER_H
