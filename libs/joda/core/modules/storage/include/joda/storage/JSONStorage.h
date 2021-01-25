//
// Created by Nico Schäfer on 9/7/17.
//

#ifndef JODA_JSONSTORAGE_H
#define JODA_JSONSTORAGE_H

#include <joda/container/ContainerFlags.h>
#include <joda/container/JSONContainer.h>

#include <future>
#include <mutex>
#include <unordered_map>
#include <future>
#include <joda/container/JSONContainer.h>
#include <joda/container/ContainerFlags.h>
class QueryPlan;

/**
 * Represents a logical collection of JSON documents.
 * They are either permanently named or temporary collections of result
 * documents.
 *
 */
class JSONStorage {
 public:
  virtual ~JSONStorage();
  void preparePurge();
  explicit JSONStorage(const std::string &query_string);
  /**
   * Consumes documents from the given queue and inserts them into the
   * JSONStorage
   * @param queue the queue through which the documents arrive
   */
  void insertDocumentsQueue(JsonContainerQueue::queue_t *queue);
  /**
   * Consumes documents from the given queue and inserts them into the
   * JSONStorage, while keeping statistics
   * @param queue the queue through which the documents arrive
   * @param insertedDocs Number of inserted documents, which will be increased
   * by the JSONStorage
   * @param insertedConts Number of inserted containers, which will be increased
   * by the JSONStorage
   */
  void insertDocumentsQueue(JsonContainerQueue::queue_t *queue,
                            size_t &insertedDocs, size_t &insertedConts);

  /**
   * Consumes containers from the given queue and inserts them into the
   * JSONStorage
   * @param queue the queue through which the containers arrive
   * @param cf Flag signalling when insertion is finished
   * @param cs Counter of remaining containers in the queue
   */
  void insertDocuments(
      moodycamel::ConcurrentQueue<std::unique_ptr<JSONContainer>> &queue,
      const std::atomic_bool &cf, std::atomic_uint &cs);

  /**
   * Inserts a single container into the JSONStorage
   * @param cont The container to insert
   */
  void insertDocuments(std::unique_ptr<JSONContainer> &&cont);

  /**
   * Fills the queue with references to all containers within the JSONStorage
   * @param queue the queue through which the containers are sent
   */
  void getDocumentsQueue(JsonContainerRefQueue::queue_t *queue);


  /**
   * Returns the number of documents in the collection
   * @return
   */
  unsigned long size() const;
  /**
   * Returns the number of containers in the collection
   * @return
   */
  unsigned long contSize() const;

  // Memory

  /**
   * Returns the estimated memory size of the collection (in bytes)
   * @return
   */
  size_t estimatedSize() const;
  /**
   * Returns the estimated memory capacity of the collection (in bytes)
   * @return
   */

  size_t estimatedCapacity() const;

  /**
   * Frees all non-essential memory.
   * This includes all document contents (Metadata required for reparsing is
   * excluded)
   */
  void freeAllMemory();

  size_t parsedSize() const;

  /**
   * Returns the name of the JSONStorage.
   * If temporary storage, then a reserved name is returned
   * @return
   */
  const std::string &getName() const;

  /**
   * Writes all documents contained in the JSONStorage to a file
   * @file The file to write to
   */
  void writeFile(const std::string &file);
  /**
   * Writes all documents contained in the JSONStorage to multiple files
   * @file The directory to write to
   */
  void writeFiles(const std::string &file);

  /**
   * Stringifies all documents in the specified range (all by default) into a
   * list of strings.
   * @param start The start of the range (inclusive)
   * @param end The end of the range (inclusive)
   * @return a list of strings
   */
  std::vector<std::string> stringify(unsigned long start = 0,
                                     unsigned long end = ULONG_MAX);

  /**
   * Returns the internal content of a range of documents (all by default)
   * @param start The start of the range (inclusive)
   * @param end The end of the range (inclusive)
   * @return a list of RJDocument
   */
  std::vector<std::shared_ptr<RJDocument>> getRaw(
      unsigned long start = 0, unsigned long end = ULONG_MAX);

  /**
   * Accepts a subset of all documents in the Container with the given handler
   * @param handler The handler to use
   * @param start The (inclusive) start index of the documents to accept
   * @param start The (inclusive) end index of the documents to accept
   * @return a list of boolean values, one for each checked document.
   */
  template<class Handler>
  std::vector<bool> AcceptDocuments(Handler &handler, unsigned long start = 0, unsigned long end = ULONG_MAX) {
    std::lock_guard<std::mutex> lock(documentMutex);
    std::vector<bool> ret;
    if (docCount == 0) return ret;
    end = std::min(end, docCount - 1);
    if (start > end) return ret;
    auto count = (end - start) + 1;
    ret.reserve(count);
    auto it = container.begin();

    unsigned long skippedSize = 0;
    //Browse container until designated is reached
    while (start >= (*it)->size() + skippedSize) {
      skippedSize += (*it)->size();
      it++;
      DCHECK(it != container.end());
    }

    while (ret.size() < count) {
      size_t contStart = start - skippedSize;
      contStart = std::max(contStart, (size_t) 0);
      unsigned long contEnd = contStart + count - ret.size() - 1;
      if (it == container.end()) {
        DCHECK(false) << "If all Container contributed to the raws, this should not be possible";
        return ret;
      }

      if ((*it)->size() != 0) {
        auto tmp = (*it)->AcceptDocuments(handler, contStart, contEnd);
        DCHECK_EQ(tmp.size(), contEnd - contStart + 1);
        std::move(tmp.begin(), tmp.end(), std::back_inserter(ret));
        it++;
      } else {
        DLOG(INFO) << "Skipping empty container";
      }

    }
    return ret;
  }

  /**
   * Returns the directory used for temporarily serializing documents
   * @return
   */
  const std::string &getRegtmpdir() const;

  /**
   * Returns a UNIX timestamp showing when the collection was last used.
   */
  unsigned long getLastUsed() const;

 protected:

  friend QueryPlan;
  const std::vector<std::unique_ptr<JSONContainer>> &getContainer() const;

  std::string name;
  std::string regtmpdir;

  std::mutex documentMutex;
  std::vector<std::unique_ptr<JSONContainer>> container;
  unsigned long docCount = 0;

  // Multithread
  size_t threadCount(size_t containerSize) const;

  std::string getStorageID() const;
};

#endif  // JODA_JSONSTORAGE_H
