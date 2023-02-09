#ifndef JODA_PYTHONIMPORTTASK_H
#define JODA_PYTHONIMPORTTASK_H

#ifdef JODA_ENABLE_PYTHON

#include <Python.h>
#include <joda/extension/python/PythonModule.h>
#include <joda/parser/IImportSource.h>
#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/pipelineatomics/queue/PipelineIOQueue.h>
#include <joda/pipelineatomics/TaskID.h>



namespace joda::queryexecution::pipeline::tasks::load {


class PythonImportExec {
 public:
  using O = queryexecution::pipeline::queue::OutputQueueTrait<std::shared_ptr<JSONContainer>>;
  using Output = O::Output;

  PythonImportExec(const std::shared_ptr<extension::python::PythonModule>& pyModule, const std::string& paramString, const double sample = 1.0);

  bool finished() const;

  void fillBuffer(std::optional<Output>& buff, std::function<void(std::optional<Output>&)> sendPartial);

  std::string toString() const;
 private:
  std::shared_ptr<extension::python::PythonModule> pyModule;
  std::string paramString;
  double sample;
  bool isFinished = false;

  PyObject* pStateOwned = nullptr;

  void init_state();
};

using PythonImport = OExecutor<PythonImportExec, tasks::AsyncType::SingleThreaded>;

}  // namespace joda::queryexecution::pipeline::tasks::load

JODA_REGISTER_PIPELINE_TASK_IN_NS(PythonImport, "PythonImport",
                                  joda::queryexecution::pipeline::tasks::load)

#endif  // JODA_ENABLE_PYTHON

#endif  // JODA_PYTHONIMPORTTASK_H
