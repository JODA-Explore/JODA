//
// Created by Nico Schäfer on 3/20/17.
//

#ifndef JODA_RAPIDJSONDOCUMENT_H
#define JODA_RAPIDJSONDOCUMENT_H

#include <memory>
#include <rapidjson/fwd.h>
#include <rapidjson/allocators.h>
#include "IOrigin.h"
#include "joda/misc/RJFwd.h"
#include "../../../../../misc/include/joda/misc/FileNameRepo.h"

typedef unsigned long DOC_ID; //The type used to identify documents

/**
 * The wrapper class of JSON documents, parsed into the internal rapidJSON format
 */
class RapidJsonDocument {
 public:
  /**
   * Creates a new RapidJsonDocument
   * @param id The ID of the document
   * @param origin The origin of the document
   * @param json The actual JSON content
   */
  RapidJsonDocument(unsigned long id,
                    std::shared_ptr<RJDocument> &&json,
                    std::unique_ptr<const IOrigin> &&origin);

  /**
   * Move constructor
   * @param doc other document
   */
   RapidJsonDocument(RapidJsonDocument &&doc) noexcept ;
  /**
   * Copy constructor
   * @param doc The RapidJsonDocument to copy from
   */
  RapidJsonDocument(const RapidJsonDocument &doc);

  /**
   * Creates an empty dummy RapidJsonDocument
   */
  RapidJsonDocument();
  /**
   * Returns the internal representation
   * @return The internal RJDocument
   */
  std::shared_ptr<RJDocument> getJson() const;
  /**
   * Sets the internal RJDocument
   * @param json The document to store
   */
  void setJson(const std::shared_ptr<RJDocument> &json);
  /**
 * Sets the internal RJDocument
 * @param json The document to store
 */
  void setJson(std::shared_ptr<RJDocument> &&json);

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
   * Returns the DOC_ID of the document
   * @return the DOC_ID of the document
   */
  unsigned long getId() const;

  const IOrigin* const getOrigin() const;

  RapidJsonDocument& operator=(const RapidJsonDocument& other);
  RapidJsonDocument& operator=(RapidJsonDocument&& other) noexcept;


 protected:
  unsigned long id;
  std::unique_ptr<const IOrigin> origin;
  std::shared_ptr<RJDocument> json;

};

typedef std::unique_ptr<RapidJsonDocument> RJDocumentPointer;
typedef std::unique_ptr<RJMemoryPoolAlloc> RJMemoryPoolPointer;

#endif //JODA_RAPIDJSONDOCUMENT_H
