#ifndef JODA_PIPELINE_LINESEPARATEDSTREAMREADER_H
#define JODA_PIPELINE_LINESEPARATEDSTREAMREADER_H

#include <joda/parser/ParserIntermediates.h>

#include <filesystem>
#include <memory>

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/pipelineatomics/queue/PipelineIOQueue.h>


namespace joda::queryexecution::pipeline::tasks::load {

class LineSeparatedStreamReaderExec {
 public:
  typedef queue::InputQueueTrait<std::pair<joda::docparsing::StreamOrigin,double>> I;
  typedef queue::OutputQueueTrait<std::vector<joda::docparsing::StringOrigin>> O;

  typedef I::Input Input;
  typedef O::Output Output;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1; 
  static constexpr size_t OutBulk = 512; 


  LineSeparatedStreamReaderExec();
  LineSeparatedStreamReaderExec(const LineSeparatedStreamReaderExec& o);

  void fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff, std::function<void(std::optional<Output>&)> sendPartial);

  void finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial){};

 private:
  int index = 0;
};

typedef IOExecutor<
    LineSeparatedStreamReaderExec, tasks::AsyncType::SingleThreaded>
    LineSeparatedStreamReader;

}  // namespace joda::queryexecution::pipeline::tasks::load

JODA_REGISTER_PIPELINE_TASK_IN_NS(LineSeparatedStreamReader, "LSStreamReader",
                                  joda::queryexecution::pipeline::tasks::load)

#endif  // JODA_PIPELINE_LINESEPARATEDSTREAMREADER_H