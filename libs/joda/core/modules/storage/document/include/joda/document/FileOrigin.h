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
  typedef std::pair<FILEID, std::pair<size_t, size_t>> ParseInterval;

  FileOrigin(FILEID id);
  FileOrigin(FILEID file, size_t start, size_t end, int index);
  /*
   * Inherited
   */
  bool isReparsable() const override;
  std::unique_ptr<IOrigin> clone() const override;
  virtual std::unique_ptr<IDPositionOrigin> cloneSpecific() const override;
  std::unique_ptr<RJDocument> reparse(RJMemoryPoolAlloc &alloc) const override;
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

  /**
   * Merges a list of ParseIntervals to the minimal possible list
   * @param intervals the list of intervals to merge
   * @return the merged list
   */
  static std::vector<ParseInterval> mergeIntervals(std::vector<ParseInterval> &&intervals);
  /**
   * Parses a list of ParseIntervals to a list of RJDocuments
   * @param alloc The memory allocator to use
   * @param intervals the list of intervals to parse
   * @return the list of parsed documents
   */
  static std::vector<std::unique_ptr<RJDocument>> parseIntervals(RJMemoryPoolAlloc &alloc, std::vector<ParseInterval> &&intervals);
};

#endif  // JODA_FILEORIGIN_H
