//
// Created by Nico Schäfer on 08/01/18.
//

#include "../include/joda/join/FileJoinManager.h"
#include <glog/logging.h>
#include <joda/fs/DirectoryRegister.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <filesystem>
#include <fstream>

JODA_JOINID FileJoinManager::getJoinVal(const RJValue& val) {
  // Check for bool
  if (val.IsBool()) {
    if (val.GetBool()) {
      if (!trueValCreated) {
        createFile(getFile(TRUE_VAL), true);
        fLock[TRUE_VAL];
        trueValCreated = true;
      }
      return TRUE_VAL;
    }
    if (!falseValCreated) {
      createFile(getFile(FALSE_VAL), true);
      fLock[FALSE_VAL];
      falseValCreated = true;
    }
    return FALSE_VAL;
  }

  /*
//Check for doubleNumber
if(val.IsDouble()){
  auto d = val.GetString();
  //Check if already hashed
  auto fIt = dMap.find(d);
  if(fIt != dMap.end()){
    return fIt->second;
  }else{
    auto id = ++currID;
    dMap[d] = id;
    return id;
  }
}
 */

  // Check for integer number
  if (val.IsInt64()) {
    auto d = val.GetInt64();
    // Check if already hashed
    auto fIt = lMap.find(d);
    if (fIt != lMap.end()) {
      return fIt->second;
    }
    auto id = ++currID;
    lMap[d] = id;
    createFile(getFile(id), d);
    fLock[id];
    return id;
  }

  // Check for String
  if (val.IsString()) {
    auto s = val.GetString();
    // Check if already hashed
    auto fIt = sMap.find(s);
    if (fIt != sMap.end()) {
      return fIt->second;
    }
    auto id = ++currID;
    sMap[s] = id;
    createFile(getFile(id), std::string(s));
    fLock[id];
    return id;
  }
  return NOT_HASHABLE;
}

bool FileJoinManager::createFile(const std::string& file, bool val) {
  std::ofstream o(file);
  o << "BOOL" << '\n' << (val ? "true" : "false") << std::endl;
  o.close();
  return true;
}

bool FileJoinManager::createFile(const std::string& file, long val) {
  std::ofstream o(file);
  o << "INT" << '\n' << val << std::endl;
  o.close();
  return true;
}

bool FileJoinManager::createFile(const std::string& file,
                                 const std::string& val) {
  std::ofstream o(file);
  o << "STR" << '\n' << val << std::endl;
  o.close();
  return true;
}

std::string FileJoinManager::getFile(JODA_JOINID id) {
  return baseDir + "/" + std::to_string(id);
}

FileJoinManager::FileJoinManager(
    const std::string& name,
    std::unique_ptr<joda::query::IValueProvider>&& valProv)
    : JoinManager(name, std::move(valProv)) {
  LOG(INFO) << "Registering join-directory";
  auto dir = joda::filesystem::DirectoryRegister::getInstance().getTMPDir(name);
  if (dir) {
    baseDir = *dir;
  } else {
    throw joda::filesystem::DirectoryAlreadyInUseException();
  }
}

void FileJoinManager::join(const RapidJsonDocument& doc) {
  // Initialize Pointer
  RJMemoryPoolAlloc alloc;
  const RJValue* valPtr = nullptr;
  RJValue val;
  if (valProv->isAtom()) {
    val = valProv->getAtomValue(doc, alloc);
    valPtr = &val;
  } else {
    valPtr = valProv->getValue(doc, alloc);
  }
  if (valPtr == nullptr) {
    return;
  }

  // Get File
  mut.lock();  // Get Lock
  auto id = getJoinVal(*valPtr);
  if (id == NOT_HASHABLE) {
    return;
  }
  auto file = getFile(id);
  auto& file_lock = fLock[id];
  mut.unlock();

  // Write to file
  file_lock.lock();
  appendDocToFile(doc, file);
  file_lock.unlock();
}

bool FileJoinManager::appendDocToFile(const RapidJsonDocument& doc,
                                      const std::string& file) {
  std::ofstream o;
  o.open(file, std::ofstream::app);
  if (!o.is_open()) {
    LOG(ERROR) << "Could not open file '" << file << "'.";
    return false;
  }

  // Get rapidjson string
  rapidjson::StringBuffer b;
  rapidjson::Writer<rapidjson::StringBuffer> w(b);
  doc.Accept(w);

  o << b.GetString() << std::endl;  // Write to file

  o.close();  // Close file
  return true;
}

FileJoinManager::~FileJoinManager() {
  LOG(INFO) << "Cleaning up join-directory";
  joda::filesystem::DirectoryRegister::getInstance().removeDirectory(baseDir);
}

const std::string& FileJoinManager::getBaseDir() const { return baseDir; }

std::vector<std::string> FileJoinManager::getJoinFiles()  {
  std::lock_guard<std::mutex> lock(mut);

  std::vector<std::string> files;
  for (auto& f : fLock) {
    files.emplace_back(getFile(f.first));
  }
  return files;
};