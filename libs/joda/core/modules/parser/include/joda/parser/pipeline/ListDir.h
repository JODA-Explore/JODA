#ifndef JODA_PIPELINE_LISTDIR_H
#define JODA_PIPELINE_LISTDIR_H

#include <filesystem>
#include <memory>

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/pipelineatomics/queue/PipelineIOQueue.h>
#include <joda/pipelineatomics/TaskID.h>



namespace joda::queryexecution::pipeline::tasks::load {

template<bool Lineseparated>
class ListDirExec {
 public:
  typedef queue::OutputQueueTrait<std::pair<std::filesystem::path,double>> O;

  typedef O::Output Output;


  ListDirExec(const std::string& dirname, const double sample = 1.0);

  bool finished() const;

  void fillBuffer(std::optional<Output>& buff, std::function<void(std::optional<Output>&)> sendPartial);

  std::string toString() const;

 private:
  std::string dirname;
  double sample;
};

typedef OExecutor<ListDirExec<false>, tasks::AsyncType::SingleThreaded> ListDir;
typedef OExecutor<ListDirExec<true>, tasks::AsyncType::SingleThreaded> LSListDir;

}  // namespace joda::queryexecution::pipeline::tasks::load

JODA_REGISTER_PIPELINE_TASK_IN_NS(LSListDir, "LSListDir",
                                  joda::queryexecution::pipeline::tasks::load)
JODA_REGISTER_PIPELINE_TASK_IN_NS(ListDir, "ListDir",
                                  joda::queryexecution::pipeline::tasks::load)
#endif  // JODA_PIPELINE_LISTDIR_H