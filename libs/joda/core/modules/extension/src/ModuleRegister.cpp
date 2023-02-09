#include <joda/extension/ModuleRegister.h>
#include <joda/extension/python/PythonAggregator.h>
#include <joda/extension/python/PythonExecutor.h>
#include <joda/extension/python/PythonImportSource.h>
#include <joda/extension/python/PythonValueProvider.h>
#include <joda/extension/python/PythonExport.h>
#include <joda/extension/ModuleType.h>

#include <filesystem>
#include <ranges>

namespace joda::extension {

ModuleRegister::ModuleRegister() {
#ifdef JODA_ENABLE_PYTHON
  // Initialize the Python Interpreter
  Py_Initialize();
  // Release GIL
  global_py_state = PyEval_SaveThread();
#endif
}

ModuleRegister::~ModuleRegister() {
  DLOG(INFO) << "Destroying ModuleRegister";
#ifdef JODA_ENABLE_PYTHON
  // Remove all Python modules
  python_modules.clear();
  //  Get GIL
  PyEval_RestoreThread(global_py_state);
  // Finalize the Python Interpreter
  Py_Finalize();
#endif
}

void ModuleRegister::registerModule(const std::string &modulePath) {
  std::filesystem::path path(modulePath);
  if (!std::filesystem::exists(path)) {
    throw ModuleDoesNotExist(modulePath);
  }

  auto module_name = path.stem().string();
  std::transform(module_name.begin(), module_name.end(), module_name.begin(),
                 ::toupper);

#ifdef JODA_ENABLE_PYTHON
  if (path.extension() == ".py") {
    // TODO already existing modules
    //  TODO Unknown module types?
    //  Get GIL
    auto gstate = PyGILState_Ensure();
    try {
      auto mod = std::make_shared<python::PythonModule>(modulePath);
      if (mod->getType() == ModuleType::UNKNOWN){
        throw UnknownModuleType(modulePath);
      }
      // Initialize module
      python_modules.try_emplace(
          module_name, std::move(mod));
    } catch (const python::PythonModuleException &e) {
      LOG(ERROR) << "Error while initializing module: " << e.what();
    }


    // Release GIL
    PyGILState_Release(gstate);


    LOG(INFO) << "Registered Python module '" << module_name << "'";
    return;
  }
#endif

  throw UnknownModuleLanguage(modulePath);
}

std::unique_ptr<query::IAggregator> ModuleRegister::getAggFunc(
    const std::string &funcName, const std::string &toPointer,
    std::vector<std::unique_ptr<joda::query::IValueProvider>> &&params) const {
  // Search Python
#ifdef JODA_ENABLE_PYTHON
  if (auto it = python_modules.find(funcName); it != python_modules.end()) {
    return std::make_unique<joda::query::PythonAggregator>(
        it->second, toPointer, std::move(params));
  }
#endif  // JODA_ENABLE_PYTHON

  // Not found
  return nullptr;
}

std::vector<std::string> ModuleRegister::getAggFuncs() const {
  std::vector<std::string> funcs;
  // Collect Python Modules
#ifdef JODA_ENABLE_PYTHON
  for (auto &[name, mod] : python_modules) {
    if (mod->getType() == ModuleType::AGG) {
      funcs.push_back(name);
    }
  }
#endif  // JODA_ENABLE_PYTHON

  return funcs;
}

std::vector<std::string> ModuleRegister::getIValFuncs() const {
  std::vector<std::string> funcs;
  // Collect Python Modules
#ifdef JODA_ENABLE_PYTHON
  for (auto &[name, mod] : python_modules) {
    if (mod->getType() == ModuleType::IVAL) {
      funcs.push_back(name);
    }
  }
#endif  // JODA_ENABLE_PYTHON

  return funcs;
}

std::unique_ptr<query::IValueProvider> ModuleRegister::getIValFunc(
    const std::string &funcName,
    std::vector<std::unique_ptr<joda::query::IValueProvider>> &&params) const {
  // Search Python
#ifdef JODA_ENABLE_PYTHON
  if (auto it = python_modules.find(funcName); it != python_modules.end()) {
    return std::make_unique<joda::query::PythonValueProvider>(
        it->second, std::move(params));
  }
#endif  // JODA_ENABLE_PYTHON

  // Not found
  return nullptr;
}

std::vector<std::unique_ptr<IQueryExecutor>> ModuleRegister::getIndices(
    const joda::query::Query &q) const {
  std::vector<std::unique_ptr<IQueryExecutor>> ret;
  // Search Python
#ifdef JODA_ENABLE_PYTHON
  std::vector<std::shared_ptr<python::PythonModule>> pModules;
  for (const auto &[name, module] : python_modules) {
    if (module->getType() == ModuleType::INDEX) {
      pModules.push_back(module);
    }
  }
  if (!pModules.empty()) {
    ret.emplace_back(
        std::make_unique<python::PythonExecutor>(q, std::move(pModules)));
  }
#endif  // JODA_ENABLE_PYTHON
  return ret;
}

bool ModuleRegister::importExists(const std::string &moduleName) const {
  return moduleExists(moduleName, ModuleType::IMPORT);
}

std::unique_ptr<docparsing::IImportSource> ModuleRegister::getImportSource(
    const std::string &moduleName, const std::string &param,
    double sample) const {
#ifdef JODA_ENABLE_PYTHON
  if (auto it = python_modules.find(moduleName); it != python_modules.end()) {
    return std::make_unique<docparsing::PythonImportSource>(it->second, param,
                                                            sample);
  }
#endif  // JODA_ENABLE_PYTHON
  return nullptr;
}

bool ModuleRegister::exportExists(const std::string &moduleName) const {
  return moduleExists(moduleName, ModuleType::EXPORT);
}

bool ModuleRegister::moduleExists(const std::string &moduleName,
                                  ModuleType type) const {
  bool ret = false;

// Search Python
#ifdef JODA_ENABLE_PYTHON
  if (const auto &mod = python_modules.find(moduleName);
      mod != python_modules.end()) {
    ret = mod->second->getType() == type;
  }
#endif  // JODA_ENABLE_PYTHON
  return ret;
}

std::unique_ptr<IExportDestination> ModuleRegister::getExportDestination(
      const std::string &moduleName,const std::string& param) const {
#ifdef JODA_ENABLE_PYTHON
  if (auto it = python_modules.find(moduleName); it != python_modules.end()) {
    return std::make_unique<PythonExport>(it->second, param);
  }
#endif  // JODA_ENABLE_PYTHON
  return nullptr;
}

std::vector<ModuleSummary> ModuleRegister::getModules() const{
  std::vector<ModuleSummary> ret;
  // Collect Python Modules
#ifdef JODA_ENABLE_PYTHON
  for (auto &[name, mod] : python_modules) {
    ModuleSummary summary;
    summary.name = name;
    summary.type = mod->getType();
    summary.language = "Python";
    summary.path = mod->getPath();
    ret.emplace_back(std::move(summary));
  }
#endif  // JODA_ENABLE_PYTHON

  return ret;
}

}  // namespace joda::extension