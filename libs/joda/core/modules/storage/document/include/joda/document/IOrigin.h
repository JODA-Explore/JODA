//
// Created by Nico Schäfer on 30/07/18.
//

#ifndef JODA_IORIGIN_H
#define JODA_IORIGIN_H

#include <glog/logging.h>

#include <boost/operators.hpp>
#include <memory>

#include "joda/misc/RJFwd.h"

/**
 * IOrigin represents the origin of a Document.
 * It could be a file, a webstream, in-memory, ...
 * This class manages the reparsing (if possible) or documents that have been
 * evicted from memory.
 */
class IOrigin {
 public:
  virtual ~IOrigin() = default;

  /**
   * Returns if the IOrigin is reparsable
   * @return true, if yes, false if not
   */
  virtual bool isReparsable() const { return false; }

  /**
   * Returns a textual representation of the IOrigin
   * @return
   */
  virtual std::string toString() const = 0;

  /**
   * Reparses the origin into a new RJDocument.
   * The allocator is used to increase performance
   * @param alloc Allocator used for allocating memoty
   * @return Reparsed RJDocument, or nullpointer if not reparsable
   */
  virtual std::unique_ptr<RJDocument> reparse(RJMemoryPoolAlloc &alloc) const {
    DCHECK(isReparsable())
        << "Origin not reparsable, should be checked before calling function";
    return nullptr;
  }

  /**
   * Clones the IOrigin for use in another document
   * @return New IOrigin with the same parameters.
   */
  virtual std::unique_ptr<IOrigin> clone() const = 0;

  /*
   * Change
   */
  virtual bool operator<(const IOrigin &x) const {
    return typeid(*this).before(typeid(x));
  };

  bool operator==(const IOrigin &x) const {
    return false;
  };


  /*
   * Keep
   */
  virtual bool operator<=(const IOrigin &x) const {
    return (*this < x) || (*this == x);
  };

  virtual bool operator>(const IOrigin &x) const { return x < *this; };

  virtual bool operator>=(const IOrigin &x) const { return x <= *this; };


 protected:
};

#endif  // JODA_IORIGIN_H
