//
// Created by Nico Schäfer on 29/12/17.
//

#include <glog/logging.h>
#include "joda/misc/FileNameRepo.h"

FileNameRepo g_FileNameRepoInstance;

FILEID FileNameRepo::getNextID() {
  return ++currID;
}
FILEID FileNameRepo::addFile(const std::string &file) {
  std::lock_guard<std::mutex> guard(mut);
  auto found = file_id.find(file);
  if(found != file_id.end()){
    return found->second;
  }else{
    auto id = getNextID();
    file_id[file] = id;
    id_file[id] = file;
    return id;
  }
}
std::string FileNameRepo::getFile(FILEID id) {
  std::lock_guard<std::mutex> guard(mut);
  auto found = id_file.find(id);
  if(found != id_file.end()){
    return found->second;
  }else{
    if(id == ProjectionFileName) return "[PROJECTION]";
    if(id == TMPObjectName) return "[TEMPORARYOBJECT]";
    LOG(ERROR) << "Tried to retrieve file belonging to id " << id << ". But id is not found in repository.";
    return "";
  }
}
std::vector<std::string> FileNameRepo::getFiles(const std::unordered_set<FILEID> &ids) {
  std::lock_guard<std::mutex> guard(mut);
  std::vector<std::string> ret;
  for (auto &&id : ids) {
    auto found = id_file.find(id);
    if(found != id_file.end()){
      ret.push_back(found->second);
    }
  }
  return ret;
}
