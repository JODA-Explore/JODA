//
// Created by Nico Schäfer on 9/7/17.
//

#ifndef JODA_JSONCONTAINER_H
#define JODA_JSONCONTAINER_H

#include <joda/config/config.h>
#include <joda/document/RapidJsonDocument.h>
#include <joda/document/view/ViewLayer.h>
#include <joda/indexing/AdaptiveIndexQuery.h>
#include <joda/indexing/QueryCache.h>
#include <joda/extension/ModuleExecutorStorage.h>
#include <joda/query/project/IProjector.h>
#include <joda/query/project/ISetProjector.h>
#include <limits.h>

#include <boost/dynamic_bitset.hpp>
#include <cstdio>
#include <functional>
#include <unordered_set>
#include <vector>

#include "../../../../document/src/DocumentCostHandler.h"
#include "joda/misc/bloom_filter.hpp"

class DataContext;

using DocIndex = boost::dynamic_bitset<>;

class ContainerIndex {
 public:
  virtual ~ContainerIndex() = default;
  virtual unsigned long estimateWork(const AdaptiveIndexQuery &query) = 0;
  virtual std::shared_ptr<const DocIndex> executeQuery(
      const AdaptiveIndexQuery &query, DataContext &dataContext) = 0;
};

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
  void preparePurge();

  /**
   * @brief Returns the ID of the container
   * @return The ID of the container
   */
  unsigned long getContainerID() const;

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
   * @param doc The actual rapidjson document
   * @param origin The origin of the document
   */
  void insertDoc(std::unique_ptr<RJDocument> &&doc,
                 std::unique_ptr<IOrigin> &&origin, size_t baseIndex = 0);

  /**
   * Inserts a document into the container using its representation class
   * @param doc The document to be inserted
   * @param baseIndex the index of the parent document (if exists)
   */
  void insertDoc(RapidJsonDocument &&doc, size_t baseIndex = 0);

  /**
   * Sets documents defined in the docs parameter
   */
  void setDocuments(const std::vector<size_t> &docs,
                    std::vector<RapidJsonDocument *> &documents, bool isRange,
                    bool loadedCheck);

  /**
   * Sets documents defined in the docs parameter
   */
  void setDocuments(const DocIndex &docs,
                    std::vector<RapidJsonDocument *> &documents,
                    bool loadedCheck);

  // Document Access
  /**
   * Executes "func" on all documents. If it returns True, the document id is
   * added to a set and returned.
   * @param func The function to execute on the documents
   * @return A set containing all document ids where the function returned True
   */
  std::unique_ptr<const DocIndex> checkDocuments(
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
   * Executes a function on all documents and discards their return value
   * @tparam  F the function to call
   * @param func The function to execute on all documents.
   */
  template <class F>
  void forAll(F &f) {
    ScopedRef ref(this);
    for (auto &&doc : docs) {
      if (doc.isValid()) {
        f(doc);
      }
    }
    setLastUsed();
  }

  /**
   * Executes a function on all documents and discards their return value
   * @tparam  F the function to call
   * @param func The function to execute on all documents.
   */
  template <class F>
  void forAll(F &&f) {
    ScopedRef ref(this);
    for (auto &&doc : docs) {
      if (doc.isValid()) {
        f(doc);
      }
    }
    setLastUsed();
  }

  /**
   * Executes a function on all documents and discards their return value
   * @tparam  F the function to call
   * @param func The function to execute on all documents.
   * @param ids The documents to check
   */

  template <class F>
  void forAll(F f, const DocIndex &ids) {
    ScopedRef useCont(this, false);
    reparseSubset(ids);

    for (size_t i = 0; i < docs.size(); ++i) {
      if (!ids[i]) continue;
      auto &doc = docs[i];
      if (doc.isValid()) {
        f(doc,i);
      }
    }
    setLastUsed();
  }

  /**
   * Executes a function on all documents and stores their return value
   * @tparam  F the function to call
   * @param func The function to execute on all documents.
   * @param ids The documents to check
   * @param vec The vector of return types to fill
   */
  template <class F, class R>
  std::vector<R> forAllRet(F f, const DocIndex &ids) {
    auto ret = std::vector<R>();
    ret.reserve(ids.size());

    ScopedRef useCont(this, false);
    reparseSubset(ids);

    for (size_t i = 0; i < docs.size(); ++i) {
      if (!ids[i]) {
        ret.emplace_back();
        continue;
      };
      auto &doc = docs[i];
      if (doc.isValid()) {
        ret.emplace_back(f(doc, i));
      }
    }
    setLastUsed();
    return ret;
  }

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
  std::vector<std::unique_ptr<RJDocument>> projectDocuments(
      const DocIndex &ids,
      const std::vector<std::unique_ptr<joda::query::IProjector>> &proj,
      RJMemoryPoolAlloc &alloc,
      const std::vector<std::unique_ptr<joda::query::ISetProjector>> &setProj);

  /**
   * Creates a view from the current container
   * @param ids The documents to project
   * @param proj A vector containing all projections to be executed on the
   * documents
   * @param alloc An allocator used to project the documents
   * @param setProj A vector containing all set-projections to be executed on
   * the documents
   * @return A vector containing all projected documents
   */
  std::unique_ptr<JSONContainer> createViewFromContainer(
      const DocIndex &ids,
      const std::vector<std::unique_ptr<joda::query::IProjector>> &proj,
      const std::vector<std::unique_ptr<joda::query::ISetProjector>> &setProj);

  DocumentCostHandler createTempViewDocs(
      const DocIndex &ids,
      const std::vector<std::unique_ptr<joda::query::IProjector>> &proj,
      const std::vector<std::unique_ptr<joda::query::ISetProjector>> &setProj);

  bool useViewBasedOnSample(
      const DocIndex &ids,
      const std::vector<std::unique_ptr<joda::query::IProjector>> &proj,
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
   * Creates the internal view objects for all documents (if required)
   */
  void setViews();
  void removeViews();

  /**
   * Reparses the contained documents
   * @param start Start index of documents to reparse
   * @param end End index of documents to reparse
   */
  void reparseSubset(unsigned long start = 0, unsigned long end = ULONG_MAX);

  /**
   * Reparses the contained documents
   * @param index The document index of all documents to be parsed
   */
  void reparseSubset(const DocIndex &index);

  void reparseSubset(const std::vector<size_t> &ids, bool isRange);

  /**
   * Checks if the container is reparsable
   */
  bool isReparsable();

  /**
   * Returns the estimated size of the container (in Bytes)
   * @return Estimated size of the container (in Bytes)
   */
  size_t estimatedSize() const;

  /**
   * Returns the parsed size of the container (in Bytes)
   * @return parsed size of the container (in Bytes)
   */
  size_t parsedSize() const;

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
  std::vector<std::unique_ptr<RJDocument>> getRaw(
      unsigned long start = 0, unsigned long end = ULONG_MAX);
  /**
   * Returns the raw document representation of the chosen documents
   * @param ids IDs of desired documents
   * @return Vector of raw documents
   */
  std::vector<std::unique_ptr<RJDocument>> getRaw(const DocIndex &ids);
  std::vector<std::unique_ptr<RJDocument>> getRaw(const DocIndex &ids,
                                                  RJMemoryPoolAlloc &alloc);

  template <class Handler>
  std::vector<bool> AcceptDocuments(Handler &handler, unsigned long start = 0,
                                    unsigned long end = ULONG_MAX) {
    std::vector<bool> ret;
    if (docs.empty()) return ret;
    ScopedRef useCont(this, false);
    reparseSubset(start, end);
    end = std::min(end, docs.size() - 1);
    if (start > end) return ret;
    for (unsigned long i = start; i <= end; ++i) {
      auto &doc = docs[i];
      if (doc.isValid()) {
        if (isView()) {
          auto &view = doc.getView();
          ret.push_back(view->Accept(handler));
        } else {
          ret.push_back(doc.getJson()->Accept(handler));
        }
      } else
        ret.push_back(false);
    }
    return ret;
  }

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

  /*
   * Returns the AdaptiveIndexManager
   * @return AdaptiveIndexManager
   */
  const std::unique_ptr<ContainerIndex> &getAdaptiveIndex() const;
  const void setAdaptiveIndex(std::unique_ptr<ContainerIndex> index);

  /**
   * Returns the storage for external indexing modules
   * @return module storage
   */
  const std::unique_ptr<joda::extension::ModuleExecutorStorage> &getModuleStorage() const;
  /**
   * Returns a list of all document ids
   * @return
   */
  std::unique_ptr<const DocIndex> getAllIDs() const;

  /**
   * Materializes the given attributes if the container is a view
   * @param atts
   */
  void materializeAttributes(const std::vector<std::string> &atts);
  /**
   * Materializes the given attributes if the container is a view and the
   * attributes cannot be returned without materialization
   * @param atts
   */
  void materializeAttributesIfRequired(const std::vector<std::string> &atts);
  /**
   * Materializes the view completly and turns the container to a normal
   * container.
   */
  void materializeView();

  /**
   *
   * @return true if the container is a queue
   */
  bool isView() const;

  /**
   * Returns a UNIX timestamp of the last time the JSONContainer was used
   * @return
   */
  unsigned long getLastUsed() const;

  bool isBaseContainer(const JSONContainer *cont) const;

  /*
   * Ref Counting
   */
  class ScopedRef {
   public:
    ScopedRef(JSONContainer *cont, bool parsed = true)
        : engaged_(true), cont_(cont) {
      cont_->useCont(parsed);
    }

    ~ScopedRef() {
      if (engaged_) {
        release();
      }
    }

    void release() {
      engaged_ = false;
      cont_->unUseCont();
    }

   private:
    bool engaged_;
    JSONContainer *cont_ = nullptr;
  };

  inline auto useContInScope(bool parse = true) {
    return ScopedRef(this, parse);
  }

 private:
  // ID
  unsigned long contId;
  static std::atomic<unsigned long> contIdCounter;

  size_t lastParsedSize = 0;
  /*
   * Ref Counting
   */
  std::atomic<unsigned int> usage{0};

  inline void useCont(bool parse = true) {
    if (isView()) baseContainer->useCont(parse);
    auto prev = usage.fetch_add(1);
    if (parse) reparse();
    if (prev == 0) setViews();
  }

  inline void unUseCont() {
    if (isView()) baseContainer->unUseCont();
    auto prev = usage.fetch_sub(1);
    if (prev == 1 && !config::storeJson) removeDocuments();
  }

  // Indices
  DOC_ID minID = std::numeric_limits<DOC_ID>::max();
  DOC_ID maxID = 0;
  bloom_filter attr_bloom;
  bool bloomCalculated = false;
  bool viewsComputed = false;
  std::unique_ptr<QueryCache> cache;
  std::unique_ptr<ContainerIndex> adaptiveIndex;
  std::unique_ptr<joda::extension::ModuleExecutorStorage> moduleStorage;

  void calculateBloom();
  void recursiveBloomAttrSearch(const RJValue &obj,
                                const std::string &attr = "");

  bool serializeMissing();
  FILEID id = 0;

  /**
   * View
   */
  std::vector<std::string> materializedAttributes;
  JSONContainer *baseContainer = nullptr;
  std::vector<JSONContainer *> subContainers;
  std::unique_ptr<ViewStructure> viewStruc;

  void addSubContainer(JSONContainer *cont);
  void removeSubContainer(JSONContainer *cont);

  /**
   * Inserts a document into the container using its raw components
   * @param doc The actual rapidjson document
   * @param origin The origin of the document
   */
  void insertViewDoc(std::unique_ptr<RJDocument> &&doc, size_t baseIndex);

  // Size
  size_t maxSize;
  size_t theoreticalSize = 0;

  // Storage
  bool final = false;
  unsigned long lastUsed = 0;
  bool deleted = false;

  void setLastUsed();

  static bool compareDocContainer(const RapidJsonDocument &i,
                                  const RapidJsonDocument &j) {
    return i.getOrigin()->operator<(*j.getOrigin());
  }

  std::vector<RapidJsonDocument> docs;
  std::vector<size_t> baseIds;

  RJMemoryPoolPointer alloc;
};

template <class retType>
void JSONContainer::forAll(std::function<retType(RapidJsonDocument &)> &func,
                           std::vector<retType> &vec) {
  ScopedRef ref(this);
  for (auto &&doc : docs) {
    if (doc.isValid()) {
      vec.push_back(std::move(func(doc)));
    }
  }
  setLastUsed();
}

#endif  // JODA_JSONCONTAINER_H
