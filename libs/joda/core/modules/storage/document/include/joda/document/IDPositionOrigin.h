//
// Created by Nico on 09/05/2019.
//

#ifndef JODA_IDPOSITIONORIGIN_H
#define JODA_IDPOSITIONORIGIN_H

#include <joda/misc/FileNameRepo.h>

#include "IOrigin.h"

/**
 * Interface representing a origin that can be represented by an ID and two
 * offsets (for example files/static webfiles/...)
 */
class IDPositionOrigin : public IOrigin {
 public:
  IDPositionOrigin(FILEID id);
  IDPositionOrigin(FILEID id, long start, long end, int index);
  std::shared_ptr<RJDocument> reparse(RJMemoryPoolAlloc &alloc) const override;
  bool isReparsable() const override;
  ~IDPositionOrigin() override = default;

  long getStart() const;
  void setStart(long start);
  long getEnd() const;
  void setEnd(long end);
  int getIndex() const;
  void setIndex(int index);

  /**
   * Returns a human readable representation of the ID (without the offsets)
   * @return
   */
  virtual std::string getStreamName() const = 0;
  std::string toString() const override = 0;
  std::unique_ptr<IOrigin> clone() const override = 0;

  bool operator<(const IOrigin &x) const override;
  bool operator==(const IOrigin &x) const override;

 protected:
  FILEID id;
  long start;
  long end;
  int index;
};

#endif  // JODA_IDPOSITIONORIGIN_H
