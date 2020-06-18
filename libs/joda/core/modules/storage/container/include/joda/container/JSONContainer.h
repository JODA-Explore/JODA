//
// Created by Nico Schäfer on 9/7/17.
//

#ifndef JODA_JSONCONTAINER_H
#define JODA_JSONCONTAINER_H

#include <joda/document/RapidJsonDocument.h>
#include <joda/indexing/QueryCache.h>
#include <joda/query/project/IProjector.h>
#include <joda/query/project/ISetProjector.h>
#include <limits.h>

#include <cstdio>
#include <functional>
#include <unordered_set>
#include <vector>

#include "joda/misc/bloom_filter.hpp"

typedef std::vector<bool> DocIndex;

/**
 * JSONContainers are a collection of JSONDocuments and indices created on them.
 * They are used as interface for all query operations.
 */
class JSONContainer {
 public:
  /**
   * Creates a default empty JSONContainer with the maximum size configured in
   * the config
   */
  JSONContainer();
  /**
   * Creates a default empty JSONContainer with the maximum size specified in
   * the parameter
   * @param maxsize Maximum size (in Bytes) of the container
   */
  explicit JSONContainer(size_t maxsize);
  JSONContainer(JSONContainer &other) = delete;

  virtual ~JSONContainer();

  /**
   * Checks if the container has enough space to add another document of size
   * "size".
   * @param size The size of the document(s) to add
   * @return True if there is enough space, False if not
   */
  bool hasSpace(size_t size) const;

  /**
   * Checks if the container has enough space to add another document of size
   * "size". If it returns True, "size" is automatically added to an internal
   * counter, to keep track of used metadata space
   * @param size The size of the document(s) to add
   * @return True if there is enough space, False if not
   */
  bool hasMetaSpace(size_t size);

  /**
   * Returns the amount of documents stored in the container
   * @return Amount of documents stored in the container
   */
  unsigned long size() const;

  /**
   * Returns the maximum size (in Bytes) of the container
   * @return Maximum size (in Bytes) of the container
   */
  size_t getMaxSize() const;

  /**
   * Uses an internal bloom filter to check if a given attribute is probably
   * contained within the document-set in the container
   * @param attr The attribute to check for
   * @return True if it is probably contained within the container, False if it
   * definitely is not
   */
  bool probContainsAttr(const std::string &attr) const;

  // Allocator
  /**
   * Returns the allocator used to store all the JSONDocuments
   * @return Pointer to the allocator
   */
  RJMemoryPoolAlloc *getAlloc();

  // Documents
  /**
   * Inserts a document into the container using its raw components
   * @param id The unique ID of the document
   * @param doc The actual rapidjson document
   * @param origin The origin of the document
   */
  void insertDoc(DOC_ID id, std::shared_ptr<RJDocument> &&doc,
                 std::unique_ptr<IOrigin> &&origin);

  /**
   * Inserts a document into the container using its representation class
   * @param doc The document to be inserted
   */
  void insertDoc(RapidJsonDocument &&doc);

  // Document Access
  /**
   * Executes "func" on all documents. If it returns True, the document id is
   * added to a set and returned.
   * @param func The function to execute on the documents
   * @return A set containing all document ids where the function returned True
   */
  std::shared_ptr<const DocIndex> checkDocuments(
      std::function<bool(RapidJsonDocument &, size_t)> &func);

  /**
   * Executes a function on all documents and stores their return value
   * @tparam retType The return-type of the function
   * @param func The function to execute on all documents.
   * @param vec The vector used to store all return values in
   */
  template <class retType>
  void forAll(std::function<retType(RapidJsonDocument &)> &func,
              std::vector<retType> &vec);

  /**
   * Executes a function on all documents and stores their return value
   * @tparam retType The return-type of the function
   * @param func The function to execute on all documents.
   */
  template <class retType>
  void forAll(std::function<retType(RapidJsonDocument &)> &func);

  /**
   * Gets all documents, whose ID is contained in "ids"
   * @param ids A set of DOC_ID used to filter the documents
   * @return A vector of documents
   */
  std::vector<RapidJsonDocument> getDocuments(const DocIndex &ids);

  /**
   * Returns all documents in the container
   * @return A vector of all documents
   */
  std::vector<RapidJsonDocument> getDocuments();

  /**
   * Projects all chosen documents into a new format
   * @param ids The documents to project
   * @param proj A vector containing all projections to be executed on the
   * documents
   * @param alloc An allocator used to project the documents
   * @param setProj A vector containing all set-projections to be executed on
   * the documents
   * @return A vector containing all projected documents
   */
  std::vector<std::shared_ptr<RJDocument>> projectDocuments(
      const DocIndex &ids, const std::vector<std::unique_ptr<joda::query::IProjector>> &proj,
      RJMemoryPoolAlloc &alloc,
      const std::vector<std::unique_ptr<joda::query::ISetProjector>> &setProj);

  /**
   * Finalizes the container.
   * After finalization no further documents can be added.
   * During this step, the indices are computed.
   */
  void finalize();

  /**
   * Finalizes the container with only the stored metadata.
   * After finalization no further documents can be added.
   * During this step, the indices are computed.
   */
  void metaFinalize();

  /**
   * Removes the content of all documents.
   * The metadata is kept.
   * This function only executes if the documents are reparsable.
   */
  void removeDocuments();

  /**
   * Reparses the contained documents
   */
  void reparse();

  /**
   * Reparses the contained documents
   * @param start Start index of documents to reparse
   * @param end End index of documents to reparse
   */
  void reparseSubset(unsigned long start = 0, unsigned long end = ULONG_MAX);

  /**
   * Checks if the container is reparsable
   */
  bool isReparsable();

  /**
   * Returns the estimated size of the container (in Bytes)
   * @return Estimated size of the container (in Bytes)
   */
  size_t estimatedSize() const;

  /*
   * Stringify
   */
  /**
   * Stringifies the documents in the container
   * @param start Start index of documents to stringify
   * @param end End index of documents to stringify
   * @return Vector of string representations of the documents
   */
  std::vector<std::string> stringify(unsigned long start = 0,
                                     unsigned long end = ULONG_MAX);

  /**
   * Returns the raw document representation of the chosen documents
   * @param start Start index of documents
   * @param end End index of documents
   * @return Vector of raw documents
   */
  std::vector<std::shared_ptr<RJDocument>> getRaw(
      unsigned long start = 0, unsigned long end = ULONG_MAX);

  /**
   * Writes the documents into a file
   * @param file The file to write the documents to
   * @param append Whether to append or replace the file
   */
  void writeFile(const std::string &file, bool append);

  /*
   * Indices
   */
  /**
   * Returns the QueryCache
   * @return QueryCache
   */
  const std::unique_ptr<QueryCache> &getCache() const;
  /**
   * Returns a list of all document ids
   * @return
   */
  std::shared_ptr<const DocIndex> getAllIDs() const;

  /**
   * Returns a UNIX timestamp of the last time the JSONContainer was used
   * @return
   */
  unsigned long getLastUsed() const;

 private:
  // Indices
  DOC_ID minID = 0;
  DOC_ID maxID = 0;
  bloom_filter attr_bloom;
  bool bloomCalculated = false;
  std::unique_ptr<QueryCache> cache;

  void calculateBloom();
  void recursiveBloomAttrSearch(const RJValue &obj,
                                const std::string &attr = "");

  bool serializeMissing();
  FILEID id = 0;

  /**
   * Serializes the container to a local file
   * @param file
   * @param id
   * @return
   */
  bool serialize(const std::string &file, FILEID id);

  // Size
  size_t maxSize;
  size_t theoreticalSize = 0;

  // Storage
  bool final = false;
  unsigned long lastUsed = 0;
  bool deleted = false;

  void setLastUsed();

  struct DocContainer {
    explicit DocContainer(const RapidJsonDocument &doc) noexcept : doc(doc) {}
    explicit DocContainer(RapidJsonDocument &&doc) noexcept
        : doc(std::move(doc)) {}
    DocContainer(const DocContainer &doc) = default;

    DocContainer(DocContainer &&doc) noexcept
        : doc(std::move(doc.doc)), valid(doc.valid) {}
    DocContainer &operator=(DocContainer &&doc) noexcept {
      valid = doc.valid;
      this->doc = std::move(doc.doc);
      return *this;
    }
    RapidJsonDocument doc;
    bool valid = true;

    bool isValid() const { return valid; }

    void remove() {
      valid = false;
      doc.removeDoc();
    }
  };

  static bool compareDocContainer(const DocContainer &i,
                                  const DocContainer &j) {
    if (!i.isValid() || !j.isValid()) return false;
    return i.doc.getOrigin() < j.doc.getOrigin();
  }

  std::vector<DocContainer> docs;

  RJMemoryPoolPointer alloc;
};

template <class retType>
void JSONContainer::forAll(std::function<retType(RapidJsonDocument &)> &func,
                           std::vector<retType> &vec) {
  reparse();
  for (auto &&doc : docs) {
    if (doc.valid) {
      vec.push_back(std::move(func(doc.doc)));
    }
  }
  setLastUsed();
};

template <class retType>
void JSONContainer::forAll(std::function<retType(RapidJsonDocument &)> &func) {
  reparse();
  for (auto &&doc : docs) {
    if (doc.valid) {
      func(doc.doc);
    }
  }
  setLastUsed();
};

#endif  // JODA_JSONCONTAINER_H
