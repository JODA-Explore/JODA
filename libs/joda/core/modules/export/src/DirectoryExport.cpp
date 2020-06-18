//
// Created by Nico on 17/04/2019.
//

#include "joda/export/DirectoryExport.h"


const std::string DirectoryExport::getTimerName() {
  return "Directory Export";
}

DirectoryExport::DirectoryExport(const std::string &dirname) : dirname(dirname) {
  //TODO Check if dir exists
}

void DirectoryExport::exportContainer(std::unique_ptr<JSONContainer> &&cont) {
  std::stringstream stream;
  std::hash<size_t> hash;

  stream << std::hex << hash((size_t)cont.get());
  std::string filename( stream.str() );
  filename += ".json";
  cont->writeFile(dirname+"/"+filename,true);
}

void DirectoryExport::consume(JsonContainerQueue::queue_t &queue) {
  auto exec = std::make_unique<IOThreadPool<DirectoryExportThread>>(&queue, nullptr, dirname);
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

  auto tok = IQueue::ctok_t(iqueue->queue);
  bool finishedItself = false;
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
