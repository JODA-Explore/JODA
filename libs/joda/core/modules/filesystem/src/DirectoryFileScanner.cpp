//
// Created by Nico Schäfer on 11/22/17.
//

#include "../include/joda/fs/DirectoryFileScanner.h"
#include <dirent.h>
#include <glog/logging.h>
#include <sys/stat.h>
#include <boost/algorithm/string.hpp>
#include <experimental/filesystem>

bool joda::filesystem::DirectoryFileScanner::fileIsValid(
    const std::string& file) {
  struct stat buffer {};
  return (stat(file.c_str(), &buffer) == 0);
}
std::vector<std::string> joda::filesystem::DirectoryFileScanner::getFilesInDir(
    const std::string& directory, const std::string& ending) {
#ifdef WINDOWS
  using namespace std;
  std::vector<string> out;
  HANDLE dir;
  WIN32_FIND_DATA file_data;

  if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) ==
      INVALID_HANDLE_VALUE)
    return; /* No files found */

  do {
    const string file_name = file_data.cFileName;

    const string full_file_name = directory + "/" + file_name;
    const bool is_directory =
        (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

    if (file_name[0] == '.') continue;

    if (is_directory) {
      auto res = getFilesInDir(full_file_name, ending);
      std::move(res.begin(), res.end(), std::back_inserter(out));
    } else {
      if (!hasEnding(file_name, ending)) continue;
      out.push_back(full_file_name);
    }

  } while (FindNextFile(dir, &file_data));

  FindClose(dir);
  return out;
#else
  using namespace std;
  std::vector<string> out;
  DIR* dir;
  struct dirent* ent;
  struct stat st {};
  dir = opendir(directory.c_str());
  if (dir == nullptr) {
    LOG(ERROR) << "Directory '" << directory << "' does not exist";
    return out;
  }
  while ((ent = readdir(dir)) != nullptr) {
    const string file_name = ent->d_name;
    const string full_file_name =
        directory +
        ((!directory.empty() && directory[directory.size() - 1] != '/') ? "/"
                                                                        : "") +
        file_name;

    if (file_name[0] == '.') {
      continue;
    }

    if (stat(full_file_name.c_str(), &st) == -1) {
      continue;
    }

    const bool is_directory = (st.st_mode & S_IFDIR) != 0;

    if (is_directory) {
      auto res = getFilesInDir(full_file_name, ending);
      std::move(res.begin(), res.end(), std::back_inserter(out));
    } else {
      if (!hasEnding(file_name, ending)) {
        continue;
      }
      out.push_back(full_file_name);
    }
  }
  closedir(dir);
  return out;
#endif
}
bool joda::filesystem::DirectoryFileScanner::hasEnding(
    const std::string& file, const std::string& ending) {
  if (ending.empty()) {
    return true;
  }
  return boost::iequals(file.substr(file.find_last_of('.') + 1), ending);
}

size_t joda::filesystem::DirectoryFileScanner::getFileSize(
    const std::string& file) const {
  namespace fs = std::experimental::filesystem;
  auto p = fs::path(file.c_str());
  if (fs::is_regular_file(p)) {
    return fs::file_size(p);
  }
  return 0;
}

size_t joda::filesystem::DirectoryFileScanner::getDirectorySize(
    const std::string& dir, bool recursive = false,
    const std::string& extension = "") const {
  namespace fs = std::experimental::filesystem;
  auto p = fs::path(dir.c_str());
  if (!fs::is_directory(p)) {
    return 0;
  }

  size_t ret = 0;
  if (recursive) {
    for (fs::recursive_directory_iterator it(p);
         it != fs::recursive_directory_iterator(); ++it) {
      if (fs::is_regular_file(*it) &&
          (extension.empty() ||
           it->path().extension().string() == "." + extension)) {
        ret += fs::file_size(*it);
      }
    }
  } else {
    for (fs::directory_iterator it(p); it != fs::directory_iterator(); ++it) {
      if (fs::is_regular_file(*it) &&
          (extension.empty() ||
           it->path().extension().string() == "." + extension)) {
        ret += fs::file_size(*it);
      }
    }
  }

  return ret;
}

std::vector<std::string> joda::filesystem::DirectoryFileScanner::getFilesInDir(
    const std::string& dir) {
  return getFilesInDir(dir, "");
}
