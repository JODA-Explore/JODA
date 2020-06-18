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

/**
 * Represents a logical collection of JSON documents.
 * They are either permanently named or temporary collections of result
 * documents.
 *
 */
class JSONStorage {
 public:
  virtual ~JSONStorage();
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
   * Returns a new unique DOC_ID
   * @return
   */
  static DOC_ID getID();

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
  size_t estimatedCapacity();

  /**
   * Frees all non-essential memory.
   * This includes all document contents (Metadata required for reparsing is
   * excluded)
   */
  void freeAllMemory();

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
   * Returns the directory used for temporarily serializing documents
   * @return
   */
  const std::string &getRegtmpdir() const;

  /**
   * Returns a UNIX timestamp showing when the collection was last used.
   */
  unsigned long getLastUsed() const;

 protected:
  std::string name;
  std::string regtmpdir;

  static std::atomic_ulong currID;
  std::mutex documentMutex;
  std::vector<std::unique_ptr<JSONContainer>> container;
  unsigned long docCount = 0;

  // Multithread
  size_t threadCount(size_t containerSize) const;

  std::string getStorageID() const;
};

#endif  // JODA_JSONSTORAGE_H
