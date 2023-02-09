//
// Created by Nico Schäfer on 3/20/17.
//

#ifndef JODA_RAPIDJSONDOCUMENT_H
#define JODA_RAPIDJSONDOCUMENT_H
#include <joda/document/view/ViewLayer.h>
#include <rapidjson/allocators.h>
#include <rapidjson/fwd.h>
#include <atomic>
#include <memory>
#include "../../../../../misc/include/joda/misc/FileNameRepo.h"
#include "IOrigin.h"
#include "joda/misc/RJFwd.h"

typedef unsigned long DOC_ID;  // The type used to identify documents

/**
 * The wrapper class of JSON documents, parsed into the internal rapidJSON
 * format
 */
class RapidJsonDocument {
 public:
  /**
   * Creates a new RapidJsonDocument
   * @param id The ID of the document
   * @param origin The origin of the document
   * @param json The actual JSON content
   */
  RapidJsonDocument(unsigned long id, std::unique_ptr<RJDocument> &&json,
                    std::unique_ptr<const IOrigin> &&origin);

  /**
   * Creates a new RapidJsonDocument
   * @param origin The origin of the document
   * @param json The actual JSON content
   */
  RapidJsonDocument(std::unique_ptr<RJDocument> &&json,
                    std::unique_ptr<const IOrigin> &&origin);

  ~RapidJsonDocument() = default;

  /**
   * Move constructor
   * @param doc other document
   */
  RapidJsonDocument(RapidJsonDocument &&doc) noexcept = default;
  RapidJsonDocument &operator=(RapidJsonDocument &&other) noexcept = default;


  /**
   * Copy constructor
   * @param doc The RapidJsonDocument to copy from
   */
  RapidJsonDocument(const RapidJsonDocument &doc) = delete;
  RapidJsonDocument &operator=(const RapidJsonDocument &other) = delete;


  /**
   * Creates an empty dummy RapidJsonDocument
   */
  RapidJsonDocument();
  /**
   * Returns the internal representation
   * @return The internal RJDocument
   */
  const std::unique_ptr<RJDocument> &getJson() const;

  /**
   * Sets the internal RJDocument
   * @param json The document to store
   */
  void setJson(std::unique_ptr<RJDocument> &&json);

  /**
   * Sets the Origin of the document
   * @param orig The new origin
   */
  void setOrigin(std::unique_ptr<const IOrigin> &&orig);

  /**
   * Deletes the internal json document
   */
  void removeDoc();

  /**
   * returns a unique document ID
   * @return unique document ID
   */
  static DOC_ID getNewID();

  /**
   * Returns the DOC_ID of the document
   * @return the DOC_ID of the document
   */
  unsigned long getId() const;

   /**
   *
   * @return true, if the document contains a valid json document
   */
  bool isValid() const;

  /**
   *
   * @return true, if the document is a view
   */
  bool isView() const;

  void setView(std::unique_ptr<ViewLayer> &&view);

  const std::unique_ptr<ViewLayer> &getView() const;

  RJValue const *Get(const RJPointer &ptr) const;

  const std::unique_ptr<const IOrigin>& getOrigin() const;



  template <typename Handler>
  bool Accept(Handler &handler) const {
    if (isView()) {
      DCHECK(view != nullptr) << "View was not computed";
      return view->Accept(handler);
    } else {
      DCHECK(json != nullptr) << "Json was not loaded";
      return json->Accept(handler);
    }
  }
  
  // Implement generatpr
  template <typename Handler>
  bool operator()(Handler &handler) const {
    return Accept(handler);
  }

 protected:
  unsigned long id{};
  std::unique_ptr<const IOrigin> origin;
  std::unique_ptr<RJDocument> json;
  std::unique_ptr<ViewLayer> view;
  bool isView_ = false;

 private:
  static std::atomic_ulong currID;
};

typedef std::unique_ptr<RapidJsonDocument> RJDocumentPointer;
typedef std::unique_ptr<RJMemoryPoolAlloc> RJMemoryPoolPointer;

#endif  // JODA_RAPIDJSONDOCUMENT_H
