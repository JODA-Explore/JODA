//
// Created by Nico Schäfer on 07/02/18.
//
#include <joda/concurrency/ThreadManager.h>
#include <joda/parser/ReaderParser.h>
#include "parsers/JSONMetaParser.h"
#include "parsers/JSONStreamParser.h"
#include "parsers/JSONTextParser.h"
#include "readers/BeautifulJSONFileReader.h"
#include "readers/JSONFileReader.h"
#include "readers/JSONURLReader.h"

size_t joda::docparsing::ReaderParser::getMaxThreads() const {
  return maxThreads;
}

void joda::docparsing::ReaderParser::setMaxThreads(size_t maxThreads) {
  joda::docparsing::ReaderParser::maxThreads = maxThreads;
}

size_t joda::docparsing::ReaderParser::getParsedDocs() const {
  return parsedDocs;
}

size_t joda::docparsing::ReaderParser::getParsedConts() const {
  return parsedConts;
}

bool joda::docparsing::ReaderParser::isParsing() const { return parsing; }

void joda::docparsing::ReaderParser::parse(
    const std::vector<std::unique_ptr<joda::docparsing::IImportSource>>&
        sources,
    std::shared_ptr<JSONStorage>& storage) {
  parsing = true;

  // Collect all required IDs
  std::unordered_set<ReaderID, boost::hash<ReaderID>> readerIds{};
  std::unordered_set<ParserID, boost::hash<ParserID>> parserIds{};
  for (const auto& source : sources) {
    readerIds.insert({source->getReaderFlag(), source->getParserFlag()});
    parserIds.insert({source->getParserFlag(), source->getContainerFlag()});
  }

  /*
   * Possible Parser=>Container Queues
   */
  std::unique_ptr<JsonContainerQueue::queue_t> jsonContainers;

  /*
   * Create queues.
   * Add another for loop if other types of queues are required
   */
  std::future<void> insertDocuments;
  for (const auto& parserId : parserIds) {
    if (parserId.second == JODA_JSON_CONTAINER_FLAG) {
      jsonContainers = JsonContainerQueue::getQueue(
          50 * (std::thread::hardware_concurrency()), 0,
          std::thread::hardware_concurrency());
      // Initialize Storage
      insertDocuments =
          std::async(std::launch::async,
                     [&storage, &jsonContainers, &conts = this->parsedConts,
                      &docs = this->parsedDocs]() {
                       return storage->insertDocumentsQueue(
                           jsonContainers.get(), conts, docs);
                     });
      break;
    }
  }

  /*
   * Check for missing queues
   */
  for (const auto& parserId : parserIds) {
    if (parserId.second != JODA_JSON_CONTAINER_FLAG) {
      DCHECK(false) << "Missing container queue: " << parserId.second;
    }
  }

  // Add additional queues here to check
  DCHECK(jsonContainers != nullptr) << "No container queues were created";

  startParser(sources, readerIds, parserIds, jsonContainers, insertDocuments);
  parsing = false;
}

void joda::docparsing::ReaderParser::startParser(
    const std::vector<std::unique_ptr<joda::docparsing::IImportSource>>&
        sources,
    std::unordered_set<joda::docparsing::ReaderParser::ReaderID,
                       boost::hash<ReaderID>>& readerIds,
    std::unordered_set<joda::docparsing::ReaderParser::ParserID,
                       boost::hash<ParserID>>& parserIds,
    std::unique_ptr<JsonContainerQueue::queue_t>& containerQueue,
    std::future<void>& jsonContInserter) {
  /*
   * Possible Reader=>Parser Queues
   */
  //<Queue,Producer,Consumer>

  std::tuple<std::unique_ptr<JsonTextParserQueue::queue_t>, std::vector<size_t>,
             std::vector<size_t>>
      jsonTextQueueBalance{};
  std::tuple<std::unique_ptr<JsonTextStreamParserQueue::queue_t>,
             std::vector<size_t>, std::vector<size_t>>
      jsonStreamQueueBalance{};

  /*
   * Create queues.
   * Add another for loop if other types of queues are required
   */
  for (const auto& parserId : parserIds) {
    if (parserId.first == JODA_JSON_TEXT_PARSER_FLAG) {
      std::get<0>(jsonTextQueueBalance) = JsonTextParserQueue::getQueue(
          config::parse_bulk_size * (std::thread::hardware_concurrency()), 0,
          std::thread::hardware_concurrency());
      break;
    }
  }

  for (const auto& parserId : parserIds) {
    if (parserId.first == JODA_JSON_TEXT_STREAM_PARSER_FLAG) {
      std::get<0>(jsonStreamQueueBalance) = JsonTextStreamParserQueue::getQueue(
          config::parse_bulk_size * (std::thread::hardware_concurrency()), 0,
          std::thread::hardware_concurrency());
      break;
    }
  }

  /*
   * Check for missing queues
   */
  for (const auto& parserId : parserIds) {
    if (!((parserId.first == JODA_JSON_TEXT_PARSER_FLAG) ||
          (parserId.first == JODA_JSON_TEXT_STREAM_PARSER_FLAG))) {
      DCHECK(false) << "Missing Parser queue: " << parserId.first;
    }
  }

  /*
   * Possible Parsers
   */
  std::unique_ptr<IThreadUser> jsonTextParser;
  std::unique_ptr<IThreadUser> jsonStreamParser;

  /*
   * Create Parsers.
   * Add another for loop if other types of queues are required
   */
  std::vector<size_t> threadIDs;
  {  // TextParser
    size_t estimated = 0;
    for (const auto& source : sources) {
      if ((source->getParserFlag() & JODA_JSON_TEXT_PARSER_FLAG) ==
          JODA_JSON_TEXT_PARSER_FLAG) {
        estimated += source->estimatedSize();
      }
    }
    estimated = std::max(static_cast<unsigned long> JSON_CONTAINER_DEFAULT_SIZE,
                         estimated / (maxThreads*2));

    for (const auto& parserId : parserIds) {
      if (parserId.first == JODA_JSON_TEXT_PARSER_FLAG &&
          parserId.second == JODA_JSON_CONTAINER_FLAG) {
        DCHECK(std::get<0>(jsonTextQueueBalance) != nullptr)
            << "Input queue missing for jsonTextParser";
        DCHECK(containerQueue != nullptr)
            << "Output queue missing for jsonTextParser";
        if (config::storeJson) {
          jsonTextParser =
              createParserFromConfig<joda::docparsing::JSONTextParser>(
                  std::get<0>(jsonTextQueueBalance).get(), containerQueue.get(),
                  maxThreads, estimated);
        } else {
          jsonTextParser =
              createParserFromConfig<joda::docparsing::JSONMetaParser>(
                  std::get<0>(jsonTextQueueBalance).get(), containerQueue.get(),
                  maxThreads, estimated);
        }
        DCHECK(jsonTextParser != nullptr) << "Could not create parser";
        if (jsonTextParser == nullptr) {
          return;
        }
        auto x =
            g_ThreadManagerInstance.registerThreadUser(jsonTextParser.get());
        threadIDs.push_back(x);
        std::get<2>(jsonTextQueueBalance).push_back(x);
        break;
      }
    }
  }

  {  // StreamParser
    size_t estimated = 0;
    for (const auto& source : sources) {
      if ((source->getParserFlag() & JODA_JSON_TEXT_STREAM_PARSER_FLAG) ==
          JODA_JSON_TEXT_STREAM_PARSER_FLAG) {
        estimated += source->estimatedSize();
      }
    }
    estimated = std::max(static_cast<unsigned long> JSON_CONTAINER_DEFAULT_SIZE,
                         estimated / (maxThreads*2));
    for (const auto& parserId : parserIds) {
      if (parserId.first == JSONStreamParser<>::getIQueueFlags() &&
          parserId.second == JSONStreamParser<>::getOQueueFlags()) {
        DCHECK(std::get<0>(jsonStreamQueueBalance) != nullptr)
            << "Input queue missing for jsonStreamParser";
        DCHECK(containerQueue != nullptr)
            << "Output queue missing for jsonStreamParser";
        jsonStreamParser = createParserFromConfig<JSONStreamParser>(
            std::get<0>(jsonStreamQueueBalance).get(), containerQueue.get(),
            maxThreads, estimated);

        DCHECK(jsonStreamParser != nullptr) << "Could not create parser";
        if (jsonStreamParser == nullptr) {
          return;
        }
        auto x =
            g_ThreadManagerInstance.registerThreadUser(jsonStreamParser.get());
        threadIDs.push_back(x);
        std::get<2>(jsonStreamQueueBalance).push_back(x);
        break;
      }
    }
  }

  /*
   * Check for missing parsers
   */
  for (const auto& parserId : parserIds) {
    if (!((parserId.first == JODA_JSON_TEXT_PARSER_FLAG &&
           parserId.second == JODA_JSON_CONTAINER_FLAG) ||
          (parserId.first == JODA_JSON_TEXT_STREAM_PARSER_FLAG &&
           parserId.second == JODA_JSON_CONTAINER_FLAG))) {
      DCHECK(false) << "Missing parser: " << parserId.first << ","
                    << parserId.second;
    }
  }

  startReaders(sources, readerIds, jsonTextQueueBalance, jsonStreamQueueBalance,
               threadIDs, jsonContInserter);
  jsonTextParser.reset();
  jsonStreamParser.reset();
}

void joda::docparsing::ReaderParser::startReaders(
    const std::vector<std::unique_ptr<joda::docparsing::IImportSource>>&
        sources,
    std::unordered_set<joda::docparsing::ReaderParser::ReaderID,
                       boost::hash<joda::docparsing::ReaderParser::ReaderID>>&
        readerIds,
    std::tuple<std::unique_ptr<JsonTextParserQueue::queue_t>,
               std::vector<size_t>, std::vector<size_t>>& textParserQueue,
    std::tuple<std::unique_ptr<JsonTextStreamParserQueue::queue_t>,
               std::vector<size_t>, std::vector<size_t>>& streamParserQueue,
    std::vector<size_t>& threadIDs, std::future<void>& jsonContInserter) {
  /*
   * Possible Source=>Reader Queues
   */
  auto jsonFileLineSeparatedQueue =
      createReaderQueue<JsonFileSeperatedReaderQueue>(readerIds);
  auto jsonFileBeautifiedQueue =
      createReaderQueue<JsonFileBeautifiedReaderQueue>(readerIds);
  auto jsonURLReaderQueue = createReaderQueue<JsonURLReaderQueue>(readerIds);

  /*
   * Check for missing queues
   */
  for (const auto& readerId : readerIds) {
    if (!(readerId.first == JsonFileSeperatedReaderQueue::getFlag() ||
          readerId.first == JsonFileBeautifiedReaderQueue::getFlag() ||
          readerId.first == JsonURLReaderQueue::getFlag())) {
      DCHECK(false) << "Missing Reader Queue: " << readerId.first;
    }
  }

  /*
   *  Readers
   */
  std::unique_ptr<IThreadUser> jsonFileReader =
      createReader<joda::docparsing::JSONFileReader>(
          jsonFileLineSeparatedQueue, textParserQueue, readerIds, threadIDs);
  std::unique_ptr<IThreadUser> jsonBeautifulFileReader =
      createReader<joda::docparsing::BeautifulJSONFileReader>(
          jsonFileBeautifiedQueue, streamParserQueue, readerIds, threadIDs);
  std::unique_ptr<IThreadUser> jsonURLReader =
      createReader<joda::docparsing::JSONURLReader>(
          jsonURLReaderQueue, streamParserQueue, readerIds, threadIDs);

  /*
   * Check for missing Readers
   */
  for (const auto& readerId : readerIds) {
    if (!((readerId.first == JsonFileSeperatedReaderQueue::getFlag() &&
           readerId.second == JsonTextParserQueue::getFlag()) ||
          (readerId.first == JsonFileBeautifiedReaderQueue::getFlag() &&
           readerId.second == JsonTextStreamParserQueue::getFlag()) ||
          (readerId.first == JsonURLReaderQueue::getFlag() &&
           readerId.second == JsonTextStreamParserQueue::getFlag()))) {
      DCHECK(false) << "Missing Reader: " << readerId.first << ","
                    << readerId.second;
    }
  }

  /*
   * Start
   */
  auto feeder = std::async(std::launch::async, [&]() {
    if (jsonFileLineSeparatedQueue.first != nullptr) {
      jsonFileLineSeparatedQueue.first->registerProducer();
    }
    if (jsonFileBeautifiedQueue.first != nullptr) {
      jsonFileBeautifiedQueue.first->registerProducer();
    }
    if (jsonURLReaderQueue.first != nullptr) {
      jsonURLReaderQueue.first->registerProducer();
    }
    for (auto& source : sources) {
      // Try casts
      if (source->getReaderFlag() == JsonFileSeperatedReaderQueue::getFlag()) {
        DCHECK(jsonFileLineSeparatedQueue.first != nullptr);
        source->feedSources(*jsonFileLineSeparatedQueue.first,
                            *jsonFileLineSeparatedQueue.second);
        continue;
      }
      if (source->getReaderFlag() == JsonFileBeautifiedReaderQueue::getFlag()) {
        DCHECK(jsonFileBeautifiedQueue.first != nullptr);
        source->feedSources(*jsonFileBeautifiedQueue.first,
                            *jsonFileBeautifiedQueue.second);
        continue;
      }
      if (source->getReaderFlag() == JsonURLReaderQueue::getFlag()) {
        DCHECK(jsonURLReaderQueue.first != nullptr);
        source->feedSources(*jsonURLReaderQueue.first,
                            *jsonURLReaderQueue.second);
        continue;
      }

      DCHECK(false) << "Use of unimplemented Source!";
    }
    if (jsonFileLineSeparatedQueue.first != nullptr) {
      jsonFileLineSeparatedQueue.first->producerFinished();
    }
    if (jsonFileBeautifiedQueue.first != nullptr) {
      jsonFileBeautifiedQueue.first->producerFinished();
    }
    if (jsonURLReaderQueue.first != nullptr) {
      jsonURLReaderQueue.first->producerFinished();
    }
  });

  // Start balancing
  bool stopBalanceReaderParser = false;
  auto balancer = std::async(std::launch::async, [&]() {
    if (std::get<0>(textParserQueue) != nullptr) {
      g_ThreadManagerInstance
          .balanceQueue<std::unique_ptr<JsonTextParserQueue::queue_t>>(
              std::get<1>(textParserQueue), std::get<2>(textParserQueue),
              std::get<0>(textParserQueue), stopBalanceReaderParser);
    }
  });

  /*
   * Wait for end
   */
  feeder.get();
  jsonContInserter.get();
  stopBalanceReaderParser = true;
  balancer.get();
  /*
   * Delete Reader before queues (Prevents seqfault)
   */
  g_ThreadManagerInstance.unregisterThreadUser(threadIDs);
  jsonFileReader.reset();
  jsonBeautifulFileReader.reset();
  jsonURLReader.reset();
}
