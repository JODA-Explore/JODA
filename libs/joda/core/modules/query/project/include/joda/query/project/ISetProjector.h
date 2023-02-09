//
// Created by Nico Schäfer on 11/14/17.
//

#ifndef JODA_ISetPROJECTOR_H
#define JODA_ISetPROJECTOR_H
#include <rapidjson/pointer.h>
#include <vector>
#include "joda/document/RapidJsonDocument.h"

namespace joda::query {
/**
 * Interface representing a single transformation used by the AS statement
 * This interface only represents 1:N transformations
 */
class ISetProjector {
 public:
  /**
   * Initializes one ISetProjector with a given destination pointer-string
   * @param to the pointer-string where the transformation result should be
   * stored
   */
  ISetProjector(const std::string &to) : ptr(to.c_str()), ptr_str(to){};

  virtual ~ISetProjector() = default;

  /**
   * Projects a value from a source document into the destination document.
   * @param json the source document, used to retrieve values from
   * @param newDocs the destination documents, used to store transformed values
   * in
   */
  virtual void project(const RapidJsonDocument &json,
                       std::vector<std::unique_ptr<RJDocument>> &newDocs,
                       bool view = false) = 0;

  /**
   * Returns the destination pointer string
   * @return
   */
  virtual std::string getToPointer() const final { return ptr_str; }

  /**
   * Returns the destination pointer
   * @return
   */
  RJPointer getRawToPointer() const { return ptr; }

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
   * Returns a list of attributes used by this projection
   * @return
   */
  virtual std::vector<std::string> getAttributes() const = 0;

 protected:
  RJPointer ptr;
  std::string ptr_str;

  unsigned long multiplicate(std::vector<std::unique_ptr<RJDocument>> &newDocs,
                             int times) {
    auto origDocs = newDocs.size();
    newDocs.reserve(origDocs * times);
    // Duplicate existing documents |VAL|-1 times
    for (auto i = 0; i < times - 1; ++i) {
      for (size_t j = 0; j < origDocs; ++j) {
        DCHECK(j < newDocs.size() && j >= 0) << "Stay in range of array";
        auto doc = std::make_unique<RJDocument>(&newDocs[j]->GetAllocator());
        doc->CopyFrom(*newDocs[j], doc->GetAllocator());
        newDocs.push_back(std::move(doc));
      }
    }
    return origDocs;
  }

  void fillArrayRangeWithVal(std::vector<std::unique_ptr<RJDocument>> &newDocs,
                             unsigned long origDocs, int i, RJValue &val) {
    for (unsigned long j = origDocs * i; j < origDocs * (i + 1); ++j) {
      DCHECK(j < newDocs.size() && j >= 0) <<  "Stay in range of array";
      RJValue newval;
      newval.CopyFrom(val, newDocs.front()->GetAllocator());
      ptr.Set(*newDocs[j], newval);
    }
  }
};
}  // namespace joda::query
#endif  // JODA_ISetPROJECTOR_H
