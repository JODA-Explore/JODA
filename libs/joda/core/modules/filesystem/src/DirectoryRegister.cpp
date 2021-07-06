//
// Created by Nico Schäfer on 08/01/18.
//

#include "../include/joda/fs/DirectoryRegister.h"
#include <glog/logging.h>
#include "joda/config/config.h"

joda::filesystem::DirectoryRegister::~DirectoryRegister() {
  LOG(INFO) << "Cleaning up remaining directories";
  for (auto&& dir : dirs) {
    removeDir(dir);
  }
}

bool joda::filesystem::DirectoryRegister::registerDirectory(
    const fs::path& dir, bool checkDuplicate = true) {
  std::lock_guard<std::mutex> guard(mut);
  if (checkDuplicate) {
    if (dirs.find(dir) != dirs.end()) {
      DLOG(INFO) << "Duplicate directory. Can't register";
      return false;
    }
  }

  if (createDir(dir)) {
    dirs.insert(dir);
    LOG(INFO) << "Registered directory '" << dir << "'";
    return true;
  }
  return false;
}

bool joda::filesystem::DirectoryRegister::createDir(const fs::path& dir,
                                                    bool checkExisting) {
  try {
    if (checkExisting) {
      if (fs::exists(dir)) {
        LOG(ERROR) << "File/Directory '" << dir.c_str() << "' already exists.";
        return false;
      }
    }
    fs::create_directories(dir);
    LOG(INFO) << "Created directory '" << dir << "'";
    return true;
  } catch (const fs::filesystem_error& e) {
    LOG(ERROR) << e.what();
    return false;
  }
}

bool joda::filesystem::DirectoryRegister::registerDirectory(
    const std::string& dir, bool checkDuplicate) {
  fs::path p = fs::path(dir.c_str());
  return registerDirectory(p, checkDuplicate);
}

bool joda::filesystem::DirectoryRegister::removeDirectory(const fs::path& dir) {
  std::lock_guard<std::mutex> guard(mut);
  if (dirs.find(dir) == dirs.end()) {
    LOG(WARNING) << "Directory '" << dir.c_str()
                 << "' not registered but requested to remove.";
    return true;
  }
  if (removeDir(dir)) {
    dirs.erase(dir);
    LOG(INFO) << "Unregistered directory '" << dir.c_str() << "'";
    return true;
  }
  LOG(ERROR) << "Could not remove directory '" << dir.c_str() << "'";
  return false;
}

bool joda::filesystem::DirectoryRegister::removeDirectory(
    const std::string& dir) {
  fs::path p = fs::path(dir.c_str());
  return removeDirectory(p);
}

bool joda::filesystem::DirectoryRegister::removeDir(const fs::path& dir) {
  try {
    DLOG(INFO) << "Removing directory '" << dir.c_str() << "'";
    auto count = fs::remove_all(dir);
    LOG(INFO) << "Removed directory '" << dir.c_str() << "'. " << count
              << " files and directories deleted.";
    return true;
  } catch (const fs::filesystem_error& e) {
    LOG(ERROR) << e.what();
    return false;
  }
}

std::string joda::filesystem::DirectoryRegister::getUniqueDir() {
  std::lock_guard<std::mutex> guard(mut);
  return config::tmpdir + "/UDIR_" + std::to_string(++currID);
}

boost::optional<std::string> joda::filesystem::DirectoryRegister::getTMPDir(
    const std::string& name) {
  auto dir = config::tmpdir + "/" + name;
  if (registerDirectory(dir, true)) {
    return dir;
  }
  return {};
}
