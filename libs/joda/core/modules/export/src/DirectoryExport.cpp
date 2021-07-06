//
// Created by Nico on 17/04/2019.
//

#include <utility>

#include "joda/export/DirectoryExport.h"

const std::string DirectoryExport::getTimerName() { return "Directory Export"; }

DirectoryExport::DirectoryExport(std::string dirname)
    : dirname(std::move(dirname)) {
  // TODO: Check if dir exists
}

void DirectoryExport::exportContainer(std::unique_ptr<JSONContainer>&& cont) {
  std::stringstream stream;
  std::hash<size_t> hash;

  stream << std::hex << hash((size_t)cont.get());
  std::string filename(stream.str());
  filename += ".json";
  cont->writeFile(dirname + "/" + filename, true);
}

void DirectoryExport::consume(JsonContainerQueue::queue_t& queue) {
  auto exec = std::make_unique<IOThreadPool<DirectoryExportThread>>(
      &queue, nullptr, dirname);
  exec->wait();
}

const std::string DirectoryExport::toString() {
  return "Export into Directory: " + dirname;
}

const std::string DirectoryExport::toQueryString() {
  return "STORE AS FILES \"" + dirname + "\"";
}

void DirectoryExportThread::work() {
  auto myid = std::this_thread::get_id();
  std::stringstream ss;
  ss << myid << ".json";
  auto mystring = ss.str();

  while (shouldRun) {
    if (!iqueue->isFinished()) {
      JsonContainerQueue::payload_t ref = nullptr;
      iqueue->retrieve(ref);
      if (ref != nullptr) {
        ref->writeFile(conf + "/" + mystring, true);
      }
    } else {
      shouldRun = false;
    }
  }
}
