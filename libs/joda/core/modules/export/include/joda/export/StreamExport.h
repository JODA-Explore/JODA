//
// Created by Nico on 20/03/2019.
//

#ifndef JODA_STREAMEXPORT_H
#define JODA_STREAMEXPORT_H

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

class WriteOutStreamExec {
 public:
  typedef queue::InputQueueTrait<std::vector<std::string>> I;

  typedef I::Input Input;


  WriteOutStreamExec();

  void emptyBuffer(std::optional<Input>& buff);

  void finalize();

 private:
};

typedef IExecutor<WriteOutStreamExec, tasks::AsyncType::Synchronous> WriteOutStream;

}  // namespace joda::queryexecution::pipeline::tasks::store

JODA_REGISTER_PIPELINE_TASK_IN_NS(WriteOutStream, "WriteOutStream",
                                  joda::queryexecution::pipeline::tasks::store)


//  _____ ______                       _
// |_   _|  ____|                     | |
//   | | | |__  __  ___ __   ___  _ __| |_ ___ _ __
//   | | |  __| \ \/ / '_ \ / _ \| '__| __/ _ \ '__|
//  _| |_| |____ >  <| |_) | (_) | |  | ||  __/ |
// |_____|______/_/\_\ .__/ \___/|_|   \__\___|_|
//                   | |
//                   |_|
                                

class StreamExport : public IExportDestination {
 public:
  StreamExport();
  const std::string toString() override;
  const std::string toQueryString() override;
  virtual PipelineTaskPtr getTask() const override;
 protected:
  const std::string getTimerName() override;

 private:
};

#endif  // JODA_STREAMEXPORT_H
