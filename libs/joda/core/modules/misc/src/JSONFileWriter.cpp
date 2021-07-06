//
// Created by Nico Schäfer on 11/22/17.
//

#include "../include/joda/misc/JSONFileWriter.h"
#include <glog/logging.h>
#include <cstring>
#include <fstream>
#include <iterator>
void JSONFileWriter::writeFile(const std::string& file,
                               const std::vector<std::string>& lines,
                               bool append) {
  std::ofstream newfile;
  if (append) {
    newfile.open(file.c_str(), std::ofstream::out | std::ofstream::app);
  } else {
    newfile.open(file.c_str());
  }
  if (newfile) {
    std::move(lines.begin(), lines.end(),
              std::ostream_iterator<std::string>(newfile, "\n"));
  } else {
    LOG(ERROR) << "Could not open file " << file
               << ". Error: " << strerror(errno);
  }
}
