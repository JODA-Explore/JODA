//
// Created by Nico on 20/03/2019.
//

#include "../include/joda/export/FileExport.h"



const std::string FileExport::getTimerName() {
  return "File Export";
}

FileExport::FileExport(const std::string &filename) : filename(filename) {
  //TODO test file/dir?
}

void FileExport::exportContainer(std::unique_ptr<JSONContainer> &&cont) {
  cont->writeFile(filename, true);
}

const std::string FileExport::toString() {
  return "Export into file: " + filename;
}

const std::string FileExport::toQueryString() {
  return "STORE AS FILE \"" + filename + "\"";
}