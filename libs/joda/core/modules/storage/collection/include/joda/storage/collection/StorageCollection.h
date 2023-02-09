//
// Created by Nico Schäfer on 11/16/17.
//

#ifndef JODA_STORAGECOLLECTION_H
#define JODA_STORAGECOLLECTION_H

#include <joda/join/JoinManager.h>

#include <unordered_map>

#include <joda/join/JoinManager.h>
#include <boost/functional/hash.hpp>
#include "../../../../../include/joda/storage/JSONStorage.h"

#define JODA_TEMPORARY_STORAGE_NAME "TEMPORARY_RESULT"
#define JODA_STORE_SKIPPED_QUERY_ID 0
#define JODA_STORE_EXTERNAL_RS_ID 1
#define JODA_STORE_EMPTY_RS_ID 2
#define JODA_STORE_VALID_ID_START 3

/**
 * StorageCollection represents a global repository of JSONStorage s.
 * Here temporary result Collections and permanently named Collections are
 * managed.
 */
class StorageCollection {
 public:
  /**
   * (Creates and) returns the global StorageCollection instance.
   */
  static StorageCollection &getInstance() {
    static StorageCollection instance;  // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }

  StorageCollection(StorageCollection const &) = delete;
  void operator=(StorageCollection const &) = delete;

  virtual ~StorageCollection();

  void clear();
  /**
   * Retrieves a named JSONStorage by its name.
   * If it does not exist, it returns a nullpointer
   * @param name The name of the JSONStorage
   * @return The JSONStorage if it exists, nullpointer else
   */
  std::shared_ptr<JSONStorage> getStorage(const std::string &name);
  /**
   * Checks if a given named JSONStorage exists.
   * @param name The name of the JSONStorage
   * @return True if it exists, false else
   */
  bool storageExists(const std::string &name);
  /**
   * Retrieves a named JSONStorage by its name.
   * If it does not exist, a new JSONStorage is created and returned
   * @param name The name of the JSONStorage
   * @return The JSONStorage
   */
  std::shared_ptr<JSONStorage> getOrAddStorage(const std::string &name);
  /**
   * Removes a named JSONStorage by its name.
   * @param name The name of the JSONStorage
   */
  void removeStorage(const std::string &name);
  /**
   * Returns a list of all named JSONStorage in the repository
   * @return
   */
  std::vector<std::shared_ptr<JSONStorage>> getStorages();
  /**
   * Returns a list of all temporary JSONStorage in the repository
   * @return
   */
  std::vector<std::shared_ptr<JSONStorage>> getTemporaryStorages();
  /**
   * Returns a list of all temporary JSONStorage in the repository together with
   * their result ID
   * @return
   */
  std::vector<std::pair<unsigned long, std::shared_ptr<JSONStorage>>>
  getTemporaryIDStorages();
  /**
   * Retrieves a temporary JSONStorage by its ID.
   * If it does not exist, a nullpointer returned
   * @param id The ID of the JSONStorage
   * @return The JSONStorage
   */
  std::shared_ptr<JSONStorage> getStorage(unsigned long id);
  /**
   * Adds an existing JSONStorage as temporary result.
   * @param storage The JSONStorage to add
   * @return The unique result ID
   */
  unsigned long addTemporaryStorage(
      const std::shared_ptr<JSONStorage> &storage);
  /**
   * Creates a new JSONStorage as temporary result.
   * @return The unique result ID of the created JSONStorage
   */
  unsigned long addTemporaryStorage();
  /**
   * Removes a temporary JSONStorage by its ID.
   * @param id The id of the JSONStorage
   */
  void removeStorage(unsigned long id);

  /**
   * Retrieves a running or starts a new join-process.
   * The join is identified by the joining IValueProvider
   * @param valProv the identifying IValueProvider
   * @return The JoinManager used for this join
   */
  std::shared_ptr<JoinManager> getOrStartJoin(
      std::unique_ptr<joda::query::IValueProvider> &&valProv);

  /**
   * Stops a join-process by it's identifying IValueProvider
   * @param valProv the identifying IValueProvider
   */
  void stopJoin(const joda::query::IValueProvider &valProv);
  /**
   * Stops a join-process by it's JoinManager
   * @param jm the JoinManager to stop
   */
  void stopJoin(const JoinManager &jm);

  /**
   * Ensures that the system has enough space for the comming operations.
   * Not enough space is available, JSONStorage s will be serialized to disk and
   * removed from memory according to a specified heuristic.
   * @param estimatedSize The estimated required size (in Bytes) for the coming
   * operation
   * @param withoutDependency A JSONStorage that cannot be removed from memory
   * for the coming operation.
   */
  void ensureSpace(
      unsigned long long estimatedSize,
      const std::shared_ptr<JSONStorage> &withoutDependency = nullptr);
  void addDependency(const std::shared_ptr<JSONStorage> &store,
                     const std::shared_ptr<JSONStorage> &dependson);
  size_t estimateHierarchySize(const std::shared_ptr<JSONStorage> &store) const;
  long long estimatedStorageSize() const;
  size_t parsedHierarchySize(const std::shared_ptr<JSONStorage> &store) const;
  size_t estimatedSize() const;
  size_t estimatedParsedSize() const;

 protected:
  std::vector<JSONStorage *> getDependencies(
      const std::shared_ptr<JSONStorage> &store) const;

  std::vector<std::pair<JSONStorage *, size_t>> chooseStoragesToRemove(
      long long toFree, const std::vector<JSONStorage *> &except) const;
  void orderContainerBySize(
      std::vector<std::pair<JSONStorage *, size_t>> &candidates) const;
  void orderContainerByLRU(
      std::vector<std::pair<JSONStorage *, size_t>> &candidates) const;
  void orderContainerByFIFO(
      std::vector<std::pair<JSONStorage *, size_t>> &candidates) const;
  void orderContainerByDependencies(
      std::vector<std::pair<JSONStorage *, size_t>> &candidates) const;
  void orderContainerByRandomExplorer(
      std::vector<std::pair<JSONStorage *, size_t>> &candidates) const;

  std::string getJoinName(const joda::query::IValueProvider &valProv);
  std::unordered_map<std::string, std::shared_ptr<JSONStorage>> storages;
  std::vector<std::string> storageInsertion;
  std::unordered_map<unsigned long, std::shared_ptr<JSONStorage>> resultStorage;
  std::unordered_map<std::string, std::shared_ptr<JoinManager>> runningJoins;
  std::mutex mut;
  std::atomic_ulong tmpRes{JODA_STORE_VALID_ID_START};

  std::unordered_set<
      std::pair<std::shared_ptr<JSONStorage>, std::shared_ptr<JSONStorage>>,
      boost::hash<std::pair<std::shared_ptr<JSONStorage>,
                            std::shared_ptr<JSONStorage>>>>
      dependencies;

  StorageCollection(){};

 private:
  friend class CollectionTest;
  friend class CollectionTest_FIFO_Test;
  friend class CollectionTest_LARGEST_Test;
  friend class CollectionTest_DEPENDENCIES_Test;
};

#endif  // JODA_STORAGECOLLECTION_H
