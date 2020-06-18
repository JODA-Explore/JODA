//
// Created by Nico on 17/04/2019.
//

#ifndef JODA_DIRECTORYEXPORT_H
#define JODA_DIRECTORYEXPORT_H

#include <joda/config/config.h>
#include "IExportDestination.h"
#include "joda/concurrency/IOThreadPool.h"

class DirectoryExport: public IExportDestination {
 public:
  DirectoryExport(const std::string &dirname);
  void consume(JsonContainerQueue::queue_t &queue) override;

  const std::string toString() override;
  const std::string toQueryString() override;
 protected:
  void exportContainer(std::unique_ptr<JSONContainer> &&cont) override;
  const std::string getTimerName() override;
 private:
  std::string dirname;
};

class DirectoryExportThread : public IWorkerThread<JsonContainerQueue, NullQueue, std::string>{
 public:
  using IWorkerThread::IWorkerThread;
  ~DirectoryExportThread() override = default;

  static const size_t recommendedThreads() {
    return config::parsingThreads;
  }
 protected:
  void work() override;
};

#endif //JODA_DIRECTORYEXPORT_H
