//
// Created by Nico Schäfer on 09/01/18.
//

#ifndef JODA_JOINPARSER_H
#define JODA_JOINPARSER_H

#include "joda/join/FileJoinManager.h"

#include "joda/storage/JSONStorage.h"

/**
 * This parser is responsible for parsing temporary join-files for the FileJoinManager
 */
class JoinParser {
  public:
  /**
   * Parses the joined files and creates RapidJsonDocuments and JSONContainers.
   * These container are then put into the correct JSONStorage.
   * @param jm The FileJoinManager used to manage the join
   * @param storage The storage to receive the JSONContainers
   */
  void parse(const FileJoinManager &jm, std::shared_ptr<JSONStorage> &storage);

};

#endif //JODA_JOINPARSER_H
