//
// Created by Nico on 09/05/2019.
//

#ifndef JODA_URLORIGIN_H
#define JODA_URLORIGIN_H

#include <joda/misc/FileNameRepo.h>
#include "IDPositionOrigin.h"

/**
 * Represents a document originating from an URL
 */
class URLOrigin : public IDPositionOrigin {
 public:
  URLOrigin(FILEID url);
  URLOrigin(FILEID url, long start, long end, int index);
  std::unique_ptr<RJDocument> reparse(RJMemoryPoolAlloc &alloc) const override;
  bool isReparsable() const override;
  ~URLOrigin() override = default;
  const FILEID &getUrl() const;

  std::string toString() const override;
  std::unique_ptr<IOrigin> clone() const override;
  virtual std::unique_ptr<IDPositionOrigin> cloneSpecific() const override;
  std::string getStreamName() const override;

 protected:
};

#endif  // JODA_URLORIGIN_H
