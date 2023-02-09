//
// Created by Nico Schäfer on 09/01/18.
//

#ifndef JODA_JOINPARSER_H
#define JODA_JOINPARSER_H

#include <filesystem>

#include "joda/join/FileJoinManager.h"
#include "joda/storage/JSONStorage.h"

/**
 * This parser is responsible for parsing temporary join-files for the
 * FileJoinManager
 */
class JoinParser {
 public:

  /**
   * Parses one join file and writes the document to the given JSONContainer
   * @param jm The FileJoinManager used to manage the join
   * @param file The file to join
   * @param cont The container to write the document to
   */
  void parse(const FileJoinManager &jm, const std::filesystem::path &file,
             JSONContainer &cont);
};

#endif  // JODA_JOINPARSER_H
