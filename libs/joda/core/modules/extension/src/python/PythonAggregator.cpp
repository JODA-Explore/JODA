#ifdef JODA_ENABLE_PYTHON

#include <joda/extension/python/PythonAggregator.h>
#include <joda/extension/python/PythonHandler.h>
#include <rapidjson/document.h>

#include <boost/algorithm/string.hpp>

void joda::query::PythonAggregator::merge(IAggregator* other) {
  auto* o = dynamic_cast<PythonAggregator*>(other);
  assert(o != nullptr);
  assert(getName() == o->getName());
  assert(toPointer == o->toPointer);
  // Acquire GIL
  auto gstate = PyGILState_Ensure();
  try {
    // Prepare arguments
    auto* pArgsOwn = PyTuple_New(2);
    PyTuple_SetItem(pArgsOwn, 0, pStateOwn);
    PyTuple_SetItem(pArgsOwn, 1, o->pStateOwn);
    // Finalize
    auto* pResultOwn = pyModule->callFunction(
        extension::python::PythonModule::AGG_MERGE_NAME, pArgsOwn);
    // Cleanup
    Py_DECREF(pArgsOwn);
    // Other state was moved to PyTuple and is cleaned up by previous call
    o->pStateOwn = nullptr;
    // Replace state
    pStateOwn = pResultOwn;
  } catch (const extension::python::PythonModuleException& e) {
    LOG(ERROR) << e.what();
  }
  // Release GIL
  PyGILState_Release(gstate);
}

joda::query::PythonAggregator::~PythonAggregator(){
  if(pStateOwn == nullptr) return;
  auto gstate = PyGILState_Ensure();
  Py_CLEAR(pStateOwn);
  PyGILState_Release(gstate);
}

RJValue joda::query::PythonAggregator::terminate(RJMemoryPoolAlloc& alloc) {
  RJValue ret;
  // Acquire GIL
  auto gstate = PyGILState_Ensure();
  try {
    // Prepare arguments
    auto* pArgsOwn = PyTuple_New(1);
    PyTuple_SetItem(pArgsOwn, 0, pStateOwn);
    // Finalize
    auto* result = pyModule->callFunction(
        extension::python::PythonModule::AGG_FINAL_NAME, pArgsOwn);
    // Convert result to JSON
    ret = pyModule->convert(result, alloc);
    // Cleanup
    Py_DECREF(pArgsOwn);
    pStateOwn = nullptr;
  } catch (const extension::python::PythonModuleException& e) {
    LOG(ERROR) << e.what();
  }
  // Release GIL
  PyGILState_Release(gstate);

  return ret;
}

std::unique_ptr<joda::query::IAggregator>
joda::query::PythonAggregator::duplicate() const {
  return std::make_unique<PythonAggregator>(pyModule, toPointer,
                                            duplicateParameters());
}

joda::query::PythonAggregator::PythonAggregator(
    const std::shared_ptr<extension::python::PythonModule>& pyModule,
    const std::string& toPointer,
    std::vector<std::unique_ptr<IValueProvider>>&& params)
    : IAggregator(toPointer, std::move(params)), pyModule(pyModule) {
  DCHECK_NOTNULL(pyModule);
  DCHECK(pyModule->getType() == extension::ModuleType::AGG)
      << "Module needs to be aggregation";

  auto arg_num = pyModule->getNumParams();
  if (this->params.size() != arg_num) {
    throw WrongParameterCountException(this->params.size(), arg_num, getName());
  }
  auto gstate = PyGILState_Ensure();
  pStateOwn = pyModule->callFunction(
      extension::python::PythonModule::AGG_INIT_NAME, nullptr);

  PyGILState_Release(gstate);
}

void joda::query::PythonAggregator::accumulate(const RapidJsonDocument& json,
                                               RJMemoryPoolAlloc& alloc) {
  // Fetch arguments
  std::vector<std::pair<const RJValue*, std::unique_ptr<RJValue>>> args;
  for (auto& param : params) {
    const RJValue* val;
    if (param->isAtom()) {
      auto val_= std::make_unique<RJValue>(param->getAtomValue(json, alloc));
      auto pair = std::make_pair(val_.get(),std::move(val_));
      args.emplace_back(std::move(pair));
    } else {
      val = param->getValue(json, alloc);
      args.emplace_back(val, nullptr);
      if (val == nullptr) {
        return;
      }
    }
  }

  // Acquire GIL
  auto gstate = PyGILState_Ensure();
  try {
    // Prepare arguments
    // ! This tuple STEALS the references to pStateOwn and arguments
    auto* pArgsOwn = PyTuple_New(params.size() + 1);
    PyTuple_SetItem(pArgsOwn, 0, pStateOwn);
    for (size_t i = 0; i < params.size(); ++i) {
      // Convert JSON to Python
      extension::python::handler::PyHandler handler;
      args[i].first->Accept(handler);
      auto* pTranslatedOwn = handler.root;
      PyTuple_SetItem(pArgsOwn, i + 1, pTranslatedOwn);
    }

    // Call accumulate
    auto* pResultOwn = pyModule->callFunction(
        extension::python::PythonModule::AGG_FUNC_NAME, pArgsOwn);
    // Release arguments (will release old pStateOwn and pTranslatedOwn)
    Py_DECREF(pArgsOwn);
    // Replace state with new
    pStateOwn = pResultOwn;
  } catch (const extension::python::PythonModuleException& e) {
    LOG(ERROR) << e.what();
  }
  // Release GIL
  PyGILState_Release(gstate);
}

const std::string joda::query::PythonAggregator::getName() const {
  auto name = pyModule->getName();
  boost::to_upper(name);
  return name;
}

#endif  // JODA_ENABLE_PYTHON