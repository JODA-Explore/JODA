#ifndef JODA_PIPELINE_INSTREAM_H
#define JODA_PIPELINE_INSTREAM_H

#include <joda/parser/ParserIntermediates.h>
#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>

#include <joda/pipelineatomics/queue/PipelineIOQueue.h>

#include <memory>

namespace joda::queryexecution::pipeline::tasks::load {

class InStreamExec {
 public:
  typedef queue::OutputQueueTrait<std::pair<joda::docparsing::StreamOrigin,double>> O;

  typedef O::Output Output;


  InStreamExec(const double sample = 1.0);

  bool finished() const;

  void fillBuffer(std::optional<Output>& buff,
                  std::function<void(std::optional<Output>&)> sendPartial);

 private:
  bool sent = false;
  double sample;
};

typedef OExecutor<InStreamExec, tasks::AsyncType::Synchronous> InStream;

}  // namespace joda::queryexecution::pipeline::tasks::load

JODA_REGISTER_PIPELINE_TASK_IN_NS(InStream, "InStream",
                                  joda::queryexecution::pipeline::tasks::load)


#endif  // JODA_PIPELINE_INSTREAM_H