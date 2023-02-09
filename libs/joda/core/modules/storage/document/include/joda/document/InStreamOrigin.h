//
// Created by Nico on 09/05/2019.
//

#ifndef JODA_INSTREAMORIGIN_H
#define JODA_INSTREAMORIGIN_H

#include <joda/misc/FileNameRepo.h>
#include "IDPositionOrigin.h"

/**
 * Represents a document originating from an URL
 */
class InStreamOrigin : public IDPositionOrigin {
 public:
  InStreamOrigin();
  InStreamOrigin(long start, long end, int index);
  std::unique_ptr<RJDocument> reparse(RJMemoryPoolAlloc &alloc) const override;
  bool isReparsable() const override;
  ~InStreamOrigin() override = default;

  std::string toString() const override;
  std::unique_ptr<IOrigin> clone() const override;
  virtual std::unique_ptr<IDPositionOrigin> cloneSpecific() const override;
  std::string getStreamName() const override;

 protected:
};

#endif  // JODA_INSTREAMORIGIN_H
