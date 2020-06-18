//
// Created by Nico on 05/12/2018.
//

#include "BeautifulJSONFileReader.h"

#include <joda/misc/FileNameRepo.h>
#include <joda/document/FileOrigin.h>
#include "joda/config/config.h"
#include <fstream>


joda::docparsing::BeautifulJSONFileReader::BeautifulJSONFileReader(IQueue *iqueue, OQueue *oqueue, void* ptr) : IWorkerThread(iqueue, oqueue, nullptr){
  oqueue->registerProducer();
  DLOG(INFO) << "Started BeautifulJSONFileReader";
}

joda::docparsing::BeautifulJSONFileReader::~BeautifulJSONFileReader() {
  oqueue->unregisterProducer();
  DLOG(INFO) << "Stopped BeautifulJSONFileReader";
}

void joda::docparsing::BeautifulJSONFileReader::work() {
  auto tok = OQueue::ptok_t(oqueue->queue);
  unsigned int i = 0;
  while (shouldRun) {
    if (!iqueue->isFinished()) {
      SampleFile file;
      iqueue->retrieve(file);

      //Check for empty filename
      if (file.file.empty()) {
        //LOG(WARNING) << "Empty filename was received by reader";
        continue;
      }

      //Get fileid
      auto fileID = g_FileNameRepoInstance.addFile(file.file);
      /*
         * Process file
         */
      //Open file
      auto fs = std::make_unique<std::fstream>();
      fs->open(file.file.c_str(), std::fstream::in);

      if (!*fs) {
        LOG(ERROR) << "Could not open \"" << file.file << "\" for reading (" << strerror(errno) << ").";
        continue;
      }

      if (*fs) {
        OPayload out;
        out.first = std::make_unique<FileOrigin>(fileID);
        out.second = std::move(fs);
        oqueue->send(tok, std::move(out));
      }

    } else {
      shouldRun = false;
    }
  }

  oqueue->producerFinished();
}
