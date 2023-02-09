#include <joda/extension/python/PythonGenerator.h>
#include <joda/extension/python/PythonImportTask.h>

#ifdef JODA_ENABLE_PYTHON
namespace joda::queryexecution::pipeline::tasks::load {

void PythonImportExec::fillBuffer(
    std::optional<Output>& buff,
    std::function<void(std::optional<Output>&)> sendPartial) {
  auto cont = std::make_unique<JSONContainer>();
  auto* alloc = cont->getAlloc();

  // Acquire GIL
  auto gstate = PyGILState_Ensure();

  try {
    if (pStateOwned == nullptr) {
      init_state();
    }

    while (true) {
      // Create arguments
      PyObject* pArgsOwned = PyTuple_New(1);
      Py_INCREF(pStateOwned);  // Inc state so it's not deleted later
      PyTuple_SetItem(pArgsOwned, 0, pStateOwned);  // Set state string

      // Get next document
      PyObject* pDocOwn = pyModule->callFunction(
          extension::python::PythonModule::IMPORT_GET_NEXT_NAME, pArgsOwned);
      // Clear
      Py_CLEAR(pArgsOwned);
      // TODO sample

      if (pDocOwn == nullptr || pDocOwn == Py_None) {
        buff = std::move(cont);
        Py_CLEAR(pDocOwn);
        Py_CLEAR(pStateOwned);
        isFinished = true;
        break;
      }

      auto docPtr = std::make_unique<RJDocument>(alloc);
      // Convert result to JSON
      extension::python::handler::PythonGenerator gen(pDocOwn);
      docPtr->Populate(gen);

      Py_CLEAR(pDocOwn);

      // Insert into container
      auto origin = std::make_unique<TemporaryOrigin>();
      cont->insertDoc(std::move(docPtr), std::move(origin));

      // Send if full
      if (!cont->hasSpace(0)) {
        buff = std::move(cont);
        sendPartial(buff);
        cont = std::make_unique<JSONContainer>();
        alloc = cont->getAlloc();
      }
    }
  } catch (const extension::python::PythonModuleException& e) {
    LOG(ERROR) << e.what();
    isFinished = true;
  }

  // Release GIL
  PyGILState_Release(gstate);
}

bool PythonImportExec::finished() const { return isFinished; }

PythonImportExec::PythonImportExec(
    const std::shared_ptr<extension::python::PythonModule>& pyModule,
    const std::string& paramstring, const double sample)
    : pyModule(pyModule), paramString(paramstring), sample(sample) {}

std::string PythonImportExec::toString() const {
  if (sample == 1.0) {
    return paramString;
  }
  return pyModule->getName() + "(" + paramString + ") sampled " +
         std::to_string(sample);
}

void PythonImportExec::init_state() {
  // Prepare param string
  auto* pParamStringOwn = PyUnicode_FromString(paramString.c_str());
  // Create arguments
  PyObject* pArgsOwned = PyTuple_New(1);

  PyTuple_SetItem(pArgsOwned, 0, pParamStringOwn);  // Set param string

  // Call init function
  pStateOwned = pyModule->callFunction(
      extension::python::PythonModule::IMPORT_INIT_NAME, pArgsOwned);

  Py_CLEAR(pArgsOwned);
}

}  // namespace joda::queryexecution::pipeline::tasks::load

#endif  // JODA_ENABLE_PYTHON