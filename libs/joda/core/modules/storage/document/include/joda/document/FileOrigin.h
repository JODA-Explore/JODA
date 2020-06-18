//
// Created by Nico Schäfer on 30/07/18.
//

#ifndef JODA_FILEORIGIN_H
#define JODA_FILEORIGIN_H

#include "IDPositionOrigin.h"
#include "IOrigin.h"
#include "joda/misc/FileNameRepo.h"
#include "joda/misc/RJFwd.h"

/**
 * FileOrigin represents a document originating from a file on the disk.
 * It stores the filepath and byte start/stop offsets for rapid reparsing
 */
class FileOrigin : public IDPositionOrigin {
 public:
  // Identifies an interval of a File where one/or multiple documents can be
  // parsed without skipping through the file
  typedef std::pair<FILEID, std::pair<long, long>> ParseInterval;

  FileOrigin(FILEID id);
  FileOrigin(FILEID file, long start, long end, int index);
  /*
   * Inherited
   */
  bool isReparsable() const override;
  std::unique_ptr<IOrigin> clone() const override;
  std::shared_ptr<RJDocument> reparse(RJMemoryPoolAlloc &alloc) const override;
  std::string toString() const override;
  std::string getStreamName() const override;

  /**
   * Returns the FILEID of the origin file
   * @return
   */
  FILEID getFile() const;
  /**
   * Returns a ParseIntervalParseInterval for this specific document
   * @return
   */
  ParseInterval getInterval() const;
};

#endif  // JODA_FILEORIGIN_H
