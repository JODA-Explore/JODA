#ifdef JODA_ENABLE_PYTHON

#include <joda/extension/python/PythonHandler.h>
#include <joda/extension/python/PythonValueProvider.h>
#include <rapidjson/document.h>

#include <boost/algorithm/string.hpp>

std::unique_ptr<joda::query::IValueProvider>
joda::query::PythonValueProvider::duplicate() const {
  return std::make_unique<joda::query::PythonValueProvider>(
      pyModule, duplicateParameters());
}

bool joda::query::PythonValueProvider::isConst() const { return false; }

bool joda::query::PythonValueProvider::isAtom() const { return true; }

RJValue joda::query::PythonValueProvider::getAtomValue(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& alloc) const {
  DCHECK(isAtom() && "Did not check for atom before calling");
  RJValue ret;

  // Fetch arguments
  std::vector<std::pair<const RJValue*, std::unique_ptr<RJValue>>> args;
  for (auto& param : params) {
    const RJValue* val;
    if (param->isAtom()) {
      auto val_ = std::make_unique<RJValue>(param->getAtomValue(json, alloc));
      auto pair = std::make_pair(val_.get(), std::move(val_));
      args.emplace_back(std::move(pair));
    } else {
      val = param->getValue(json, alloc);
      args.emplace_back(val, nullptr);
      if (val == nullptr) {
        return ret;
      }
    }
  }

  // Acquire GIL
  auto gstate = PyGILState_Ensure();
  try {
    // Prepare arguments
    // ! This tuple STEALS the references to pStateOwn and arguments
    auto* pArgsOwn = PyTuple_New(params.size());
    for (size_t i = 0; i < params.size(); ++i) {
      // Convert JSON to Python
      extension::python::handler::PyHandler handler;
      args[i].first->Accept(handler);
      auto* pTranslatedOwn = handler.root;
      PyTuple_SetItem(pArgsOwn, i, pTranslatedOwn);
    }

    // Call getValue
    auto* pResultOwn = pyModule->callFunction(
        extension::python::PythonModule::IVAL_GETVALUE_NAME, pArgsOwn);
    // Convert result to JSON
    ret = pyModule->convert(pResultOwn, alloc);
    // Release python result
    Py_CLEAR(pResultOwn);
    // Release arguments (will release old pStateOwn and pTranslatedOwn)
    Py_CLEAR(pArgsOwn);
  } catch (const extension::python::PythonModuleException& e) {
    LOG(ERROR) << e.what();
  }
  // Release GIL
  PyGILState_Release(gstate);

  return ret;
}
const RJValue* joda::query::PythonValueProvider::getValue(
    const RapidJsonDocument& /*json*/, RJMemoryPoolAlloc& /*alloc*/) const {
  DCHECK(!isAtom() && "Did not check for atom before calling");
  return nullptr;
}

std::string joda::query::PythonValueProvider::getName() const {
  auto name = pyModule->getName();
  boost::to_upper(name);
  return name;
}

joda::query::IValueType joda::query::PythonValueProvider::getReturnType()
    const {
  return returnType;
}

joda::query::PythonValueProvider::PythonValueProvider(
    const std::shared_ptr<extension::python::PythonModule>& pyModule,
    std::vector<std::unique_ptr<IValueProvider>>&& parameters)
    : IValueProvider(std::move(parameters)), pyModule(pyModule) {
  DCHECK_NOTNULL(pyModule);
  DCHECK(pyModule->getType() == extension::ModuleType::IVAL)
      << "Module needs to be a value provider";

  auto arg_num = pyModule->getNumParams();
  checkParamSize(arg_num);

  returnType = pyModule->getReturnType();
}

#endif  // JODA_ENABLE_PYTHON