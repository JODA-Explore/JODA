
#ifndef JODA_PYTHONEXPORT_H
#define JODA_PYTHONEXPORT_H

#include <joda/export/IExportDestination.h>
#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/pipelineatomics/PipelineTask.h>
#include <joda/extension/python/PythonModule.h>

#ifdef JODA_ENABLE_PYTHON

//  _____ _            _ _
// |  __ (_)          | (_)
// | |__) | _ __   ___| |_ _ __   ___
// |  ___/ | '_ \ / _ \ | | '_ \ / _ \.
// | |   | | |_) |  __/ | | | | |  __/
// |_|   |_| .__/ \___|_|_|_| |_|\___|
//         | |
//         |_|

namespace joda::queryexecution::pipeline::tasks::store {

class PythonExporterExec {
 public:
  using I = queue::InputQueueTrait<std::shared_ptr<JSONContainer>>;
  using Input = I::Input;


  PythonExporterExec(
      const std::shared_ptr<extension::python::PythonModule>& pyModule,
      const std::string& paramstring);

  void emptyBuffer(std::optional<Input>& buff);

  void finalize();

 private:
  std::shared_ptr<extension::python::PythonModule> pyModule;
  std::string paramString;
  PyObject* pStateOwned = nullptr;

  void init_state();
};

using PythonExporter =
    IExecutor<PythonExporterExec, tasks::AsyncType::Synchronous>;

}  // namespace joda::queryexecution::pipeline::tasks::store

JODA_REGISTER_PIPELINE_TASK_IN_NS(PythonExporter, "PythonExporter",
                                  joda::queryexecution::pipeline::tasks::store)

//  _____ ______                       _
// |_   _|  ____|                     | |
//   | | | |__  __  ___ __   ___  _ __| |_ ___ _ __
//   | | |  __| \ \/ / '_ \ / _ \| '__| __/ _ \ '__|
//  _| |_| |____ >  <| |_) | (_) | |  | ||  __/ |
// |_____|______/_/\_\ .__/ \___/|_|   \__\___|_|
//                   | |
//                   |_|

class PythonExport : public IExportDestination {
 public:
  PythonExport(const std::shared_ptr<joda::extension::python::PythonModule>& pyModule,
               const std::string& paramstring);
  const std::string toString() override;
  const std::string toQueryString() override;
  virtual PipelineTaskPtr getTask() const override;

 protected:
  const std::string getTimerName() override;

 private:
  std::shared_ptr<joda::extension::python::PythonModule> pyModule;
  std::string paramString;
};

#endif  // JODA_ENABLE_PYTHON
#endif  // JODA_PYTHONEXPORT_H
