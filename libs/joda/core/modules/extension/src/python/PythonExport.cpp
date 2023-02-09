#include <joda/extension/python/PythonExport.h>
#include <joda/extension/python/PythonHandler.h>

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
PythonExporterExec::PythonExporterExec(
    const std::shared_ptr<extension::python::PythonModule>& pyModule,
    const std::string& paramstring)
    : pyModule(pyModule), paramString(paramstring) {}

void PythonExporterExec::emptyBuffer(std::optional<Input>& buff) {
  auto cont = std::move(buff.value());
  buff.reset();
  DCHECK(cont != nullptr);

  // Acquire GIL
  auto gstate = PyGILState_Ensure();

  try {
    if (pStateOwned == nullptr) {
      init_state();
    }
    auto func = [this](const RapidJsonDocument& doc) {
      extension::python::handler::PyHandler handler;
      // Create arguments
      PyObject* pArgsOwned = PyTuple_New(2);
      Py_INCREF(pStateOwned);  // Inc state so it's not deleted later
      PyTuple_SetItem(pArgsOwned, 0, pStateOwned);  // Set state string
      // Translate document
      doc.Accept(handler);
      PyTuple_SetItem(pArgsOwned, 1, handler.root);  // Set translated doc
                                                     // set  document
      pyModule->callFunction(
          extension::python::PythonModule::EXPORT_SET_NEXT_NAME, pArgsOwned);

      // Clear
      Py_CLEAR(pArgsOwned);
    };

    cont->forAll(func);

  } catch (const extension::python::PythonModuleException& e) {
    LOG(ERROR) << e.what();
  }

  // Release GIL
  PyGILState_Release(gstate);
}

void PythonExporterExec::finalize() {
  // Acquire GIL
  auto gstate = PyGILState_Ensure();

  try {
    if (pStateOwned == nullptr) {
      init_state();
    }
    // Create arguments
    PyObject* pArgsOwned = PyTuple_New(1);

    PyTuple_SetItem(pArgsOwned, 0, pStateOwned);  // Set param string

    // Call init function
    pStateOwned = pyModule->callFunction(
        extension::python::PythonModule::EXPORT_FINALIZE_NAME, pArgsOwned);

    Py_CLEAR(pArgsOwned);
  } catch (const extension::python::PythonModuleException& e) {
    LOG(ERROR) << e.what();
  }

  // Release GIL
  PyGILState_Release(gstate);
}

void PythonExporterExec::init_state() {
  // Prepare param string
  auto* pParamStringOwn = PyUnicode_FromString(paramString.c_str());
  // Create arguments
  PyObject* pArgsOwned = PyTuple_New(1);

  PyTuple_SetItem(pArgsOwned, 0, pParamStringOwn);  // Set param string

  // Call init function
  pStateOwned = pyModule->callFunction(
      extension::python::PythonModule::EXPORT_INIT_NAME, pArgsOwned);

  Py_CLEAR(pArgsOwned);
}

}  // namespace joda::queryexecution::pipeline::tasks::store

//  _____ ______                       _
// |_   _|  ____|                     | |
//   | | | |__  __  ___ __   ___  _ __| |_ ___ _ __
//   | | |  __| \ \/ / '_ \ / _ \| '__| __/ _ \ '__|
//  _| |_| |____ >  <| |_) | (_) | |  | ||  __/ |
// |_____|______/_/\_\ .__/ \___/|_|   \__\___|_|
//                   | |
//                   |_|

const std::string PythonExport::getTimerName() { return "PythonExport"; }

PythonExport::PythonExport(
    const std::shared_ptr<joda::extension::python::PythonModule>& pyModule,
    const std::string& paramstring)
    : pyModule(pyModule), paramString(paramstring) {}

PythonExport::PipelineTaskPtr PythonExport::getTask() const {
  return std::make_unique<
      joda::queryexecution::pipeline::tasks::store::PythonExporterTask>(
      pyModule, paramString);
};

const std::string PythonExport::toString() {
  return "Export using python module " + pyModule->getName() + "(" +
         paramString + ")";
}

const std::string PythonExport::toQueryString() {
  return "STORE AS " + pyModule->getName() + " \"" + paramString + "\"";
}

#endif  // JODA_ENABLE_PYTHON