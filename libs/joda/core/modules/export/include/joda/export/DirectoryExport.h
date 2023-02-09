//
// Created by Nico on 17/04/2019.
//

#ifndef JODA_DIRECTORYEXPORT_H
#define JODA_DIRECTORYEXPORT_H

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/pipelineatomics/PipelineTask.h>

#include "IExportDestination.h"

//  _____ _            _ _
// |  __ (_)          | (_)
// | |__) | _ __   ___| |_ _ __   ___
// |  ___/ | '_ \ / _ \ | | '_ \ / _ \.
// | |   | | |_) |  __/ | | | | |  __/
// |_|   |_| .__/ \___|_|_|_| |_|\___|
//         | |
//         |_|

namespace joda::queryexecution::pipeline::tasks::store {

class WriteFilesExec {
 public:
  typedef queue::InputQueueTrait<std::vector<std::string>> I;

  typedef I::Input Input;

  // Overwrite Input Bulk Size, only read 1/4 of a block
  static constexpr size_t InputBulk = I::InputBulkSize/4; 

  WriteFilesExec(const std::string& dirname);
  WriteFilesExec(const WriteFilesExec& other);

  void emptyBuffer(std::optional<Input>& buff);

  void finalize();

 private:
  std::string dirname;
  mutable size_t num = 0;
};

typedef IExecutor<WriteFilesExec, tasks::AsyncType::MultiThreaded> WriteFiles;

}  // namespace joda::queryexecution::pipeline::tasks::store

JODA_REGISTER_PIPELINE_TASK_IN_NS(WriteFiles, "WriteFiles",
                                  joda::queryexecution::pipeline::tasks::store)

//  _____ ______                       _
// |_   _|  ____|                     | |
//   | | | |__  __  ___ __   ___  _ __| |_ ___ _ __
//   | | |  __| \ \/ / '_ \ / _ \| '__| __/ _ \ '__|
//  _| |_| |____ >  <| |_) | (_) | |  | ||  __/ |
// |_____|______/_/\_\ .__/ \___/|_|   \__\___|_|
//                   | |
//                   |_|

class DirectoryExport : public IExportDestination {
 public:
  DirectoryExport(std::string dirname);
  const std::string toString() override;
  const std::string toQueryString() override;
  virtual PipelineTaskPtr getTask() const override;

 protected:
  const std::string getTimerName() override;

 private:
  std::string dirname;
};

#endif  // JODA_FILEEXPORT_H
