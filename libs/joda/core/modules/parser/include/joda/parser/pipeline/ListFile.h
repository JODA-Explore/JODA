#ifndef JODA_PIPELINE_LISTFILE_H
#define JODA_PIPELINE_LISTFILE_H

#include <filesystem>
#include <memory>

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/pipelineatomics/queue/PipelineIOQueue.h>
#include <joda/pipelineatomics/TaskID.h>


namespace joda::queryexecution::pipeline::tasks::load {

template<bool Lineseparated>
class ListExec {
 public:
  typedef queue::OutputQueueTrait<std::pair<std::filesystem::path,double>> O;

  typedef O::Output Output;

  ListExec(const std::string& filename, const double sample = 1.0);

  bool finished() const;

  void fillBuffer(std::optional<Output>& buff, std::function<void(std::optional<Output>&)> sendPartial);

  std::string toString() const;
 private:
  std::string filename;
  double sample;
};

typedef OExecutor<ListExec<true>, tasks::AsyncType::SingleThreaded> LSListFile;
typedef OExecutor<ListExec<false>, tasks::AsyncType::SingleThreaded> ListFile;

}  // namespace joda::queryexecution::pipeline::tasks::load

JODA_REGISTER_PIPELINE_TASK_IN_NS(LSListFile, "LSListFile",
                                  joda::queryexecution::pipeline::tasks::load)
JODA_REGISTER_PIPELINE_TASK_IN_NS(ListFile, "ListFile",
                                  joda::queryexecution::pipeline::tasks::load)

#endif  // JODA_PIPELINE_LISTFILE_H