//
// Created by Nico on 27/11/2018.
//

#ifndef JODA_JSON_PIPELINE_TEXTPARSER_H
#define JODA_JSON_PIPELINE_TEXTPARSER_H

#include <joda/concurrency/IOThreadPool.h>
#include <joda/config/config.h>
#include <joda/container/ContainerFlags.h>
#include <joda/parser/ParserFlags.h>
#include <joda/parser/ParserIntermediates.h>
#include <joda/pipelineatomics/queue/DefaultPipelineQueues.h>
#include <joda/storage/JSONStorage.h>
#include <rapidjson/error/en.h>

#include "../src/parsers/ContainerScheduler/DefaultContainerScheduler.h"

namespace joda::docparsing {
/**
 * A parser which receives the textual representation of a document and parses
 * it into the in-memory format.
 * @tparam Scheduler The document scheduler to use for clustering documents into
 * containers
 */
template <class Scheduler = DefaultContainerScheduler<false>>
class JSONPipelineTextParser {
 public:
  typedef queryexecution::pipeline::queue::InputQueueTrait<std::vector<joda::docparsing::StringOrigin>>
      I;
  typedef queryexecution::pipeline::queue::OutputQueueTrait<std::shared_ptr<JSONContainer>> O;

  typedef I::Input Input;
  typedef O::Output Output;

  // Overwrite Input Bulk Size, only read 1/4 of a block
  static constexpr size_t InputBulk = I::InputBulkSize / 4;

  JSONPipelineTextParser(size_t sourceSize)
      : sourceSize(sourceSize), sched() {
    DLOG(INFO) << "Started JSONPipelineTextParser";
  }

  JSONPipelineTextParser(const JSONPipelineTextParser& o)
      : sourceSize(o.sourceSize), sched() {
    DLOG(INFO) << "Started JSONPipelineTextParser";
  }

  ~JSONPipelineTextParser() { DLOG(INFO) << "Stopped JSONPipelineTextParser"; };

  void fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff,
                  std::function<void(std::optional<Output>&)> sendPartial) {
    if(!ibuff.has_value()) {
      return;
    }

    auto buff = std::move(ibuff.value());
    ibuff.reset();

    while(!buff.empty()) {
      auto text = std::move(buff.back());
      buff.pop_back();

      if (text.stringOrigin == nullptr) {
        LOG(WARNING) << "Got empty Origin for document";
        continue;
      }
      if (text.string.empty()) {
        LOG(WARNING) << "Got empty line for document";
        continue;
      }

      // Parse line
      auto cont = sched.getContainerForDoc(text.string);
      auto doc = sched.getNewDoc(cont);
      doc->Parse(text.string.c_str());
      if (doc->HasParseError()) {
        LOG(WARNING) << std::string(
                            rapidjson::GetParseError_En(doc->GetParseError()))
                     << " with Origin: " << text.stringOrigin->toString();
        continue;
      }

      // Insert doc in container
      auto ptr = sched.scheduleDocument(cont, std::move(doc),
                                        std::move(text.stringOrigin),
                                        text.string.size());

      if (ptr != nullptr) {
        obuff= std::move(ptr);
        sendPartial(obuff);
      }
    }
  }

  void finalize(std::optional<Output>& obuff,
                  std::function<void(std::optional<Output>&)> sendPartial) {
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

#endif  // JODA_JSON_PIPELINE_TEXTPARSER_H
