//
// Created by Nico Schäfer on 12/07/18.
//
#include "../include/joda/misc/Benchmark.h"
#include <glog/logging.h>
#include <joda/config/config.h>
#include <joda/misc/RJFwd.h>
#include <joda/version.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <fstream>
#include <mutex>

Benchmark::Benchmark() : valid(config::benchmark) {
  if (valid) {
    benchDoc = RJDocument(rapidjson::kArrayType);
    currentLine = RJValue(rapidjson::kObjectType);
  }
}

Benchmark::Benchmark(const std::string& file) : benchfile(file) {
  valid = config::benchmark;
  if (!valid) {
    return;
  }
  local = file.empty();
  if (!local) {
    if (fs::exists(benchfile)) {
      parseBenchfile();
    } else {
      auto p = benchfile.parent_path();
      if (!p.empty() && !fs::exists(p)) {
        LOG(ERROR) << "Can't create benchfile, directory does not exist: " << p;
        local = true;
      }
      benchDoc = RJDocument(rapidjson::kArrayType);
    }
  } else {
    benchDoc = RJDocument(rapidjson::kArrayType);
  }

  currentLine = RJValue(rapidjson::kObjectType);
  if (!benchDoc.IsArray()) {
    LOG(ERROR) << "Benchfile does not contain Array JSON";
  }
}

void Benchmark::parseBenchfile() {
  if (local) {
    return;
  }
  std::lock_guard<std::mutex> lock(mut);
  std::ifstream file(benchfile);
  rapidjson::IStreamWrapper isw(file);
  benchDoc.ParseStream(isw);
  if (!benchDoc.HasParseError() ||
      benchDoc.GetParseError() ==
          rapidjson::ParseErrorCode::kParseErrorDocumentEmpty) {
    if (benchDoc.IsNull()) {
      benchDoc.SetArray();
    }
    valid = true;
    local = false;
  } else {
    valid = true;
    local = true;
    LOG(ERROR) << "Could not parse benchfile: "
               << rapidjson::GetParseError_En(benchDoc.GetParseError());
  }
}

void Benchmark::writeBenchfile() const {
  if (!valid || local) {
    return;
  }
  std::ofstream file(benchfile, std::ios::out);
  rapidjson::OStreamWrapper osw(file);
  rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
  benchDoc.Accept(writer);
  file.flush();
  file.close();
}

void Benchmark::finishLine() {
  if (valid) {
    std::lock_guard<std::mutex> lock(mut);
    DCHECK(benchDoc.IsArray());
    RJValue versionInfo(rapidjson::kObjectType);
    versionInfo.AddMember("Version", JODA_VERSION_STRING,
                          benchDoc.GetAllocator());
    versionInfo.AddMember("Build", JODA_GIT_REVISION, benchDoc.GetAllocator());
    versionInfo.AddMember("Build Time", JODA_BUILD_TIME,
                          benchDoc.GetAllocator());
    currentLine.AddMember("System", versionInfo, benchDoc.GetAllocator());
    benchDoc.PushBack(currentLine, benchDoc.GetAllocator());
    currentLine = RJValue(rapidjson::kObjectType);
    writeBenchfile();
  }
}

std::string Benchmark::toString() const {
  if (!valid) {
    return "";
  }
  rapidjson::StringBuffer buff;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buff);
  benchDoc.Accept(writer);
  return buff.GetString();
}

Benchmark::~Benchmark() { writeBenchfile(); }

std::string Benchmark::lastLineToString() const {
  if (valid && benchDoc.Size() > 0) {
    rapidjson::StringBuffer buff;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buff);
    benchDoc[benchDoc.Size() - 1].Accept(writer);
    return buff.GetString();
  }
  return "";
}

std::string Benchmark::currentLineToString() const {
  if (valid) {
    rapidjson::StringBuffer buff;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buff);
    currentLine.Accept(writer);
    return buff.GetString();
  }
  return "";
}

void Benchmark::addThread(double bloom, double select, double project,
                          double agg, double copy, double serialize,
                          double sample_view_cost) {
  std::lock_guard<std::mutex> lock(mut);
  RJPointer threads_ptr(THREADS);
  auto threads = threads_ptr.Get(currentLine);

  RJPointer thread_ptr(std::string(THREADS) + "/-");
  RJValue thread(rapidjson::kObjectType);

  if (bloom > 0) {
    thread.AddMember("Bloom", RJValue(bloom), benchDoc.GetAllocator());
  }
  if (select > 0) {
    thread.AddMember("Select", RJValue(select), benchDoc.GetAllocator());
  }
  if (project > 0) {
    thread.AddMember("Project", RJValue(project), benchDoc.GetAllocator());
  }
  if (agg > 0) {
    thread.AddMember("Aggregate", RJValue(agg), benchDoc.GetAllocator());
  }
  if (copy > 0) {
    thread.AddMember("Copy", RJValue(copy), benchDoc.GetAllocator());
  }
  if (serialize > 0) {
    thread.AddMember("Serialize", RJValue(serialize), benchDoc.GetAllocator());
  }
  if (sample_view_cost > 0) {
    thread.AddMember("View_Cost", RJValue(sample_view_cost),
                     benchDoc.GetAllocator());
  }

  if (threads == nullptr) {
    threads_ptr.Set(currentLine, RJValue(rapidjson::kArrayType),
                    benchDoc.GetAllocator());
  }
  thread_ptr.Set(currentLine, thread, benchDoc.GetAllocator());
}

bool Benchmark::isValid() const { return valid; }

const RJValue& Benchmark::getLastLine() const {
  return benchDoc[benchDoc.Size() - 1];
}
