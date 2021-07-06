//
// Created by Nico on 11/14/17.
//

#ifndef JODA_IPROJECTOR_H
#define JODA_IPROJECTOR_H
#include <joda/document/RapidJsonDocument.h>
#include <rapidjson/pointer.h>

namespace joda::query {
/**
 * Interface representing a single transformation used by the AS statement
 * This interface only represents 1:1 transformations
 */
class IProjector {
 public:
  /**
   * Initializes one IProjector with a given destination pointer-string
   * @param to the pointer-string where the transformation result should be
   * stored
   */
  IProjector(const std::string &to) : ptr(to.c_str()), ptr_str(to){};

  virtual ~IProjector() = default;

  /**
   * Projects a value from a source document into the destination document.
   * This function uses the (overwritten) getVal function to retrieve the value
   * and stores the result at the to-pointer in the new document
   * @param json the source document, used to retrieve values from
   * @param newDoc the destination document, used to store transformed values in
   * @param flag indicating if projecting from a view
   */
  virtual void project(const RapidJsonDocument &json, RJDocument &newDoc,
                       bool view = false) {
    auto val = this->getVal(json, newDoc.GetAllocator());
    if (!val.IsNull()) ptr.Set(newDoc, val);
  }

  /**
   * Returns the destination pointer string
   * @return
   */
  std::string getToPointer() const { return ptr_str; }
  /**
   * Returns the destination pointer
   * @return
   */
  const RJPointer &getRawToPointer() const { return ptr; }

  /**
   * Returns a string representing the transformation function
   * @return
   */
  virtual std::string getType() = 0;

  /**
   * Returns a string representing the whole transformation as it would be
   * specified in a query
   * @return
   */
  virtual std::string toString() { return "'" + ptr_str + "':"; }

  /**
   * Returns a list of attributes that have to be materialized for this
   * projection to work
   * @return
   */
  virtual std::vector<std::string> getMaterializeAttributes() const = 0;

 protected:
  virtual RJValue getVal(const RapidJsonDocument &json,
                         RJMemoryPoolAlloc &alloc) = 0;

  RJPointer ptr;
  std::string ptr_str;
};
}  // namespace joda::query
#endif  // JODA_IPROJECTOR_H
