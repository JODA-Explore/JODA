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
  IDPositionOrigin(FILEID id, size_t start, size_t end, int index);
  std::unique_ptr<RJDocument> reparse(RJMemoryPoolAlloc &alloc) const override;
  bool isReparsable() const override;
  virtual ~IDPositionOrigin() override = default;

  size_t getStart() const;
  void setStart(size_t start);
  size_t getEnd() const;
  void setEnd(size_t end);
  int getIndex() const;
  void setIndex(int index);
  FILEID getID() const;

  /**
   * Returns a human readable representation of the ID (without the offsets)
   * @return
   */
  virtual std::string getStreamName() const = 0;
  std::string toString() const override = 0;
  std::unique_ptr<IOrigin> clone() const override = 0;
  virtual std::unique_ptr<IDPositionOrigin> cloneSpecific() const  = 0;

  virtual bool operator<(const IOrigin &x) const override final;
  bool operator<(const IDPositionOrigin &other) const;
  bool operator==(const IDPositionOrigin &other) const;

 protected:
  FILEID id;
  size_t start{};
  size_t end{};
  int index{};
};

#endif  // JODA_IDPOSITIONORIGIN_H
