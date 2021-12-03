//
// Created by Nico Schäfer on 30/07/18.
//

#include "joda/document/FileOrigin.h"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <algorithm>
#include <fstream>

bool FileOrigin::isReparsable() const { return true; }

std::unique_ptr<RJDocument> FileOrigin::reparse(
    RJMemoryPoolAlloc &alloc) const {
  std::string line;
  std::ifstream f(g_FileNameRepoInstance.getFile(id));
  if (!f.is_open()) {
    LOG(ERROR) << "Could not open file: " << id;
  }

  line.resize(static_cast<unsigned long>(end - start));
  f.seekg(std::max(0l, start));  // Go to start of interval
  f.read(&line[0], end - start);

  /*
   * Parse
   */
  auto doc = std::make_unique<RJDocument>(&alloc);
  doc->Parse(line.c_str());
  if (doc->HasParseError()) {
    if (doc->GetParseError() != rapidjson::kParseErrorDocumentEmpty) {
      LOG(ERROR) << "Reparse error: "
                 << rapidjson::GetParseError_En(doc->GetParseError())
                 << " in File: " << g_FileNameRepoInstance.getFile(id) << ":"
                 << std::to_string(start) << "-" << std::to_string(end);
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

FILEID FileOrigin::getFile() const { return id; }

FileOrigin::ParseInterval FileOrigin::getInterval() const {
  return {id, {start, end}};
}

std::string FileOrigin::toString() const {
  return std::string("File: ") + std::to_string(id) + ":" +
         std::to_string(start) + "-" + std::to_string(end);
}

std::string FileOrigin::getStreamName() const {
  return std::string("File: ") + g_FileNameRepoInstance.getFile(id);
}

FileOrigin::FileOrigin(FILEID id) : IDPositionOrigin(id) {}

std::vector<FileOrigin::ParseInterval> FileOrigin::mergeIntervals(
    std::vector<FileOrigin::ParseInterval> &&intervals) {
  std::vector<FileOrigin::ParseInterval> ret;
  if (intervals.size() == 0) {
    return ret;
  }
  ret.push_back(intervals[0]);
  // Merge continuous intervals
  for (size_t i = 1; i < intervals.size(); i++) {
    if (intervals[i].first == ret.back().first &&
        intervals[i].second.first <= ret.back().second.second) {
      ret.back().second.second =
          std::max(ret.back().second.second, intervals[i].second.second);
    } else {
      ret.push_back(intervals[i]);
    }
  }
  return ret;
}

std::vector<std::unique_ptr<RJDocument>> FileOrigin::parseIntervals(
    RJMemoryPoolAlloc &alloc,
    std::vector<FileOrigin::ParseInterval> &&intervals) {
  std::vector<std::unique_ptr<RJDocument>> ret;
  if (intervals.empty()) {
    return ret;
  }
  FILEID lastFile = intervals[0].first;
  std::ifstream f =
      std::ifstream(g_FileNameRepoInstance.getFile(intervals[0].first));
  for (auto &interval : intervals) {
    
    // Close old file and open new one if interval file change
    if (lastFile != interval.first) {
      f.close();
      f = std::ifstream(g_FileNameRepoInstance.getFile(interval.first));
      lastFile = interval.first;
    }
    if (!f.is_open()) {
      LOG(ERROR) << "Could not open file: " << interval.first;
      continue;
    }

    if (f.tellg() < interval.second.first) {
      f.seekg(std::max(0l, interval.second.first));  // Go to start of interval
                                                     // if not already there
    }

    rapidjson::IStreamWrapper isw(f);
    /*
     * Parse
     */
    
    auto start = f.tellg();
    while (f.tellg() < interval.second.second-1 && f.tellg() >= 0) {
      auto doc = std::make_unique<RJDocument>(&alloc);
      // Parse all documents in the interval
      doc->ParseStream<rapidjson::kParseStopWhenDoneFlag>(isw);
      auto end = f.tellg();
      if (end < 0) {   // Sometimes seek ends at -1
        break;
      }
      if (doc->HasParseError()) {
        LOG(ERROR) << "Reparse error: "
                   << rapidjson::GetParseError_En(doc->GetParseError())
                   << " in File: "
                   << g_FileNameRepoInstance.getFile(interval.first) << ":"
                   << std::to_string(start) << "-"
                   << std::to_string(end);

        ret.push_back(nullptr);
        continue;
      }
      start = end;
      ret.push_back(std::move(doc));
    }
  }
  return ret;
}
