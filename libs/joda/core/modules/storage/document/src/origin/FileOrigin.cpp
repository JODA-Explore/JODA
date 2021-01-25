//
// Created by Nico Schäfer on 30/07/18.
//

#include "joda/document/FileOrigin.h"
#include <fstream>
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>

bool FileOrigin::isReparsable() const {
  return true;
}

std::unique_ptr<RJDocument> FileOrigin::reparse(RJMemoryPoolAlloc &alloc) const {
  std::string line;
  std::ifstream f(g_FileNameRepoInstance.getFile(id));
  if (!f.is_open())
    LOG(ERROR) << "Could not open file: " << id;

  line.resize(static_cast<unsigned long>(end - start));
  f.seekg(std::max(0l, start)); //Go to start of interval
  f.read(&line[0], end - start);


  /*
   * Parse
   */
  auto doc = std::make_unique<RJDocument>(&alloc);
  doc->Parse(line.c_str());
  if (doc->HasParseError()) {
    if (doc->GetParseError() != rapidjson::kParseErrorDocumentEmpty) {
      LOG(ERROR) << "Reparse error: " << rapidjson::GetParseError_En(doc->GetParseError()) <<
                 " in File: " << g_FileNameRepoInstance.getFile(id) << ":" << std::to_string(start) << "-"
                 << std::to_string(end);
    }
    return nullptr;
  }

  f.close();
  return doc;
}


FileOrigin::FileOrigin(FILEID file, long start, long end, int index)
    : IDPositionOrigin(file, start, end, index) {
  CHECK(FileNameRepo::validFile(file));
}

std::unique_ptr<IOrigin> FileOrigin::clone() const {
  return std::make_unique<FileOrigin>(id, start, end, index);
}



FILEID FileOrigin::getFile() const {
  return id;
}

FileOrigin::ParseInterval FileOrigin::getInterval() const {
  return {id, {start, end}};
}

std::string FileOrigin::toString() const {
  return std::string("File: ") + std::to_string(id) + ":" + std::to_string(start) + "-"
                               + std::to_string(end);
}

std::string FileOrigin::getStreamName() const {
  return std::string("File: ") + g_FileNameRepoInstance.getFile(id);
}

FileOrigin::FileOrigin(FILEID id) : IDPositionOrigin(id) {

}
