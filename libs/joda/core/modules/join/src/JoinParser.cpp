//
// Created by Nico Schäfer on 09/01/18.
//

#include "joda/join/JoinParser.h"

#include <glog/logging.h>
#include <joda/config/config.h>
#include <joda/document/RapidJsonDocument.h>
#include <joda/document/TemporaryOrigin.h>
#include <rapidjson/error/en.h>

#include <fstream>

#include "joda/concurrency/readerwriterqueue.h"
#include "joda/fs/DirectoryFileScanner.h"

void JoinParser::parse(const FileJoinManager& jm,
                       const std::filesystem::path& file, JSONContainer& cont) {

  // Open file
  std::ifstream infile(file);
  if (!infile.is_open()) {
    LOG(ERROR) << "Could no open file '" << file << "'. Skipping";
    return;
  }
  std::string header_1;

  // Get Header I: Type Info
  if (!std::getline(infile, header_1)) {
    LOG(ERROR) << "File '" << file << "' has no header. Skipping";
    return;
  }
  // Get Header II: Variable
  std::string header_2;
  if (!std::getline(infile, header_2)) {
    LOG(ERROR) << "File '" << file << "' has incorrect header. Skipping";
    return;
  }

  // Parse header info
  RJValue val;
  if (header_1 == "BOOL") {
    if (header_2 == "true") {
      val.SetBool(true);
    } else if (header_2 == "false") {
      val.SetBool(false);
    } else {
      LOG(ERROR) << "File '" << file << "' has incorrect header. Skipping";

      return;
    }
  } else if (header_1 == "INT") {
    try {
      long long num = std::atoll(header_2.c_str());
      val.SetInt64(num);
    } catch (const std::exception& e) {
      LOG(ERROR) << "File '" << file << "' has incorrect header. Skipping";

      return;
    }

  } else if (header_1 == "STR") {
    val.SetString(header_2.c_str(), *cont.getAlloc());
  } else {
    LOG(ERROR) << "File '" << file << "' has wrong header: '" << header_1
               << "'";
    return;
  }

  // Parse Join
  auto joinDoc = std::make_unique<RJDocument>(cont.getAlloc());
  joinDoc->SetObject();
  RJValue joinName;
  joinName.SetString(jm.getName().c_str(), *cont.getAlloc());
  joinDoc->AddMember(joinName, val, joinDoc->GetAllocator());
  std::string line;
  RJValue arrVal;
  arrVal.SetArray();
  while (std::getline(infile, line)) {
    RJDocument tmpDoc(cont.getAlloc());
    tmpDoc.Parse(line.c_str());
    if (tmpDoc.HasParseError()) {
      LOG(ERROR) << std::string(
                        rapidjson::GetParseError_En(tmpDoc.GetParseError()))
                 << " in File: " << file << ":" << line;

      continue;
    }
    arrVal.PushBack(std::move(tmpDoc), joinDoc->GetAllocator());
  }

  joinDoc->AddMember("joins", std::move(arrVal), joinDoc->GetAllocator());
}
