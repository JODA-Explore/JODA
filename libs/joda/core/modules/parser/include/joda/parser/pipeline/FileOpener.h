#ifndef JODA_PIPELINE_FILEOPENER_H
#define JODA_PIPELINE_FILEOPENER_H

#include <joda/document/FileOrigin.h>
#include <joda/parser/ParserIntermediates.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/pipelineatomics/queue/PipelineIOQueue.h>
#include <joda/pipelineatomics/TaskID.h>

namespace joda::queryexecution::pipeline::tasks::load {

template<bool Lineseparated>
class FileOpenerExec {
 public:
  typedef queue::InputQueueTrait<std::pair<std::filesystem::path,double>> I;
  typedef queue::OutputQueueTrait<std::pair<joda::docparsing::StreamOrigin,double>> O;

  typedef I::Input Input;
  typedef O::Output Output;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1; 

  FileOpenerExec();

  void fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff, std::function<void(std::optional<Output>&)> sendPartial);

  void finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial){};

 private:
};

typedef IOExecutor<FileOpenerExec<true>, tasks::AsyncType::SingleThreaded>
    LSFileOpener;

typedef IOExecutor<FileOpenerExec<false>, tasks::AsyncType::SingleThreaded>
    UnformattedFileOpener;

}  // namespace joda::queryexecution::pipeline::tasks::load


JODA_REGISTER_PIPELINE_TASK_IN_NS(LSFileOpener, "LSFileOpener",
                                  joda::queryexecution::pipeline::tasks::load)
JODA_REGISTER_PIPELINE_TASK_IN_NS(UnformattedFileOpener, "UnformattedFileOpener",
                                  joda::queryexecution::pipeline::tasks::load)


#endif  // JODA_PIPELINE_FILEOPENER_H