//
// Created by Nico Schäfer on 07/02/18.
//

#ifndef JODA_READERPARSER_H
#define JODA_READERPARSER_H

#include "joda/config/config.h"
#include <glog/logging.h>
#include <joda/parser/IImportSource.h>
#include <joda/storage/JSONStorage.h>
#include "../../../../storage/container/include/joda/container/ContainerFlags.h"
#include "IImportSource.h"
#include <boost/functional/hash.hpp>
#include <joda/concurrency/IThreadUser.h>
#include <joda/concurrency/IOThreadPool.h>
#include <joda/similarity/measures/AttributeJaccard.h>
#include <joda/similarity/SimilarityScheduler.h>
#include <joda/similarity/measures/PathJaccard.h>
#include <joda/concurrency/ThreadManager.h>

namespace joda::docparsing {
class ReaderParser {
public:
  typedef std::pair<ReaderFlag,ParserFlag> ReaderID;
  typedef std::pair<ParserFlag,ContainerFlag> ParserID;

  ReaderParser() = default;

  void parse(const std::vector<std::unique_ptr<IImportSource>> &sources,
             std::shared_ptr<JSONStorage> &storage);

  size_t getMaxThreads() const;
  void setMaxThreads(size_t maxThreads);
  size_t getParsedDocs() const;
  size_t getParsedConts() const;
  bool isParsing() const;

 protected:
  size_t parsedDocs = 0;
  size_t parsedConts = 0;
  bool parsing = false;

 private:

  void startParser(const std::vector<std::unique_ptr<IImportSource>> &sources,
                   std::unordered_set<ReaderParser::ReaderID, boost::hash<ReaderID>> &readerIds,
                   std::unordered_set<ReaderParser::ParserID, boost::hash<ParserID>> &parserIds,
                   std::unique_ptr<JsonContainerQueue::queue_t> &containerQueue,
                   std::future<void> &jsonContInserter);

  void startReaders(const std::vector<std::unique_ptr<IImportSource>> &sources,
                    std::unordered_set<ReaderParser::ReaderID,
                                       boost::hash<ReaderParser::ReaderID>> &readerIds,
                    std::tuple<std::unique_ptr<JsonTextParserQueue::queue_t>,
                               std::vector<size_t>,
                               std::vector<size_t>> &textParserQueue,
                    std::tuple<std::unique_ptr<JsonTextStreamParserQueue::queue_t>,
                               std::vector<size_t>,
                               std::vector<size_t>> &streamParserQueue,
                    std::vector<size_t> &threadIDs,
                    std::future<void> &jsonContInserter);

  template<template<class ...> class T, class... Args>
  inline std::unique_ptr<IThreadUser> createParserFromConfig(Args... args);

  template<class Queue>
  inline auto createReaderQueue(
      std::unordered_set<ReaderParser::ReaderID, boost::hash<ReaderParser::ReaderID>> &readerIds);

  template<class Reader, class RQueueCont, class PQueueCont>
  inline std::unique_ptr<IOThreadPool<Reader>> createReader(
      RQueueCont &rqueue,
      PQueueCont &pqueue,
      std::unordered_set<ReaderParser::ReaderID, boost::hash<ReaderParser::ReaderID>> &readerIds,
      std::vector<size_t> &threadIDs
  );

  size_t maxThreads = 1;
};

template<template<class ...> class T, class... Args>
std::unique_ptr<IThreadUser> ReaderParser::createParserFromConfig(Args... args) {
  if (config::sim_measure == config::NO_SIMILARITY) {
    return std::make_unique<IOThreadPool<T<>>>(args...);
  } else if (config::sim_measure == config::Sim_Measures::PATH_JACCARD) {
    return std::make_unique<IOThreadPool<T<SimilarityScheduler<PathJaccard>>>>(args...);
  } else if (config::sim_measure == config::Sim_Measures::ATTRIBUTE_JACCARD) {
    return std::make_unique<IOThreadPool<T<SimilarityScheduler<AttributeJaccard>>>>(args...);
  } else {
    DCHECK(false) << "Similarity Measure missing in Parser construction";
  }
  return nullptr;
}

template<typename Queue>
auto ReaderParser::createReaderQueue(
    std::unordered_set<ReaderParser::ReaderID, boost::hash<ReaderParser::ReaderID>> &readerIds) {
  std::pair<std::unique_ptr<typename Queue::queue_t>, std::unique_ptr<typename Queue::queue_t::ptok_t>> queue;
  for (const auto &readerId : readerIds) {
    if (readerId.first == Queue::getFlag()) {
      queue.first = Queue::getQueue(config::read_bulk_size * (std::thread::hardware_concurrency()),
                                    0,
                                    std::thread::hardware_concurrency());
      queue.second = std::make_unique<typename Queue::queue_t::ptok_t>(queue.first->queue);
      break;
    }
  }
  return queue;
}

template<class Reader, class RQueueCont, class PQueueCont>
std::unique_ptr<IOThreadPool<Reader>> ReaderParser::createReader(RQueueCont &rqueue,
                                                                 PQueueCont &pqueue,
                                                                 std::unordered_set<ReaderParser::ReaderID,
                                                                                    boost::hash<ReaderParser::ReaderID>> &readerIds,
                                                                 std::vector<size_t> &threadIDs) {
  const auto ReaderName = typeid(Reader).name();
  for (const auto &readerId : readerIds) {
    if (readerId.first == Reader::getIQueueFlags() && readerId.second == Reader::getOQueueFlags()) {
      DCHECK(rqueue.first != nullptr) << "Input queue missing for " << ReaderName;
      DCHECK((ReaderFlag) rqueue.first->getFlag() == Reader::getIQueueFlags())
      << "Input queue type mismatching for " << ReaderName;
      DCHECK(std::get<0>(pqueue) != nullptr) << "Output queue missing for " << ReaderName;
      DCHECK((ParserFlag) std::get<0>(pqueue)->getFlag() == Reader::getOQueueFlags())
      << "Output queue type mismatching for " << ReaderName;

      auto tmp = std::make_unique<IOThreadPool<Reader>>(rqueue.first.get(),
                                                        std::get<0>(pqueue).get(),
                                                        config::readingThreads,
                                                        nullptr);
      auto x = g_ThreadManagerInstance.registerThreadUser(tmp.get());
      threadIDs.push_back(x);
      std::get<1>(pqueue).push_back(x);
      return tmp;
    }
  }
  return nullptr;
}
}

#endif //JODA_READERPARSER_H
