//
// Created by Nico Schäfer on 11/22/17.
//

#ifndef JODA_JSONFILEWRITER_H
#define JODA_JSONFILEWRITER_H
#include <vector>
#include <string>
/**
 * A helper class serializing JSON documents into a file
 */
class JSONFileWriter {
 public:
  /**
   * Writes all text-lines contained in lines to the file.
   * @param file The file to write to
   * @param lines The lines to write
   * @param append Decides if lines are appended or the file is replaced
   */
  void writeFile(const std::string &file, const std::vector<std::string> &lines, bool append = false);

};

#endif //JODA_JSONFILEWRITER_H
