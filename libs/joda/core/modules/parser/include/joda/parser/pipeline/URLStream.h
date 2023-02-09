#ifndef JODA_PIPELINE_URLSTREAM_H
#define JODA_PIPELINE_URLSTREAM_H

#include <filesystem>
#include <memory>

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/pipelineatomics/queue/PipelineIOQueue.h>
#include <joda/pipelineatomics/TaskID.h>

#include <joda/parser/ParserIntermediates.h>

namespace joda::queryexecution::pipeline::tasks::load {

template<bool Lineseparated>
class URLStreamExec {
 public:
  typedef queue::OutputQueueTrait<std::pair<joda::docparsing::StreamOrigin,double>> O;

  typedef O::Output Output;

  URLStreamExec(const std::string& url, const double sample = 1.0);

  bool finished() const;

  void fillBuffer(std::optional<Output>& buff, std::function<void(std::optional<Output>&)> sendPartial);

 private:
  std::string url;
  double sample;
};

typedef OExecutor<URLStreamExec<true>, tasks::AsyncType::Synchronous> LSURLStream;
typedef OExecutor<URLStreamExec<false>, tasks::AsyncType::Synchronous> URLStream;

}  // namespace joda::queryexecution::pipeline::tasks::load

JODA_REGISTER_PIPELINE_TASK_IN_NS(LSURLStream, "LSURLStream",
                                  joda::queryexecution::pipeline::tasks::load)
JODA_REGISTER_PIPELINE_TASK_IN_NS(URLStream, "URLStream",
                                  joda::queryexecution::pipeline::tasks::load)

#endif  // JODA_PIPELINE_URLSTREAM_H