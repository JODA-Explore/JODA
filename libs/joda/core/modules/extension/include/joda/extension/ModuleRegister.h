#ifndef JODA_MODULEREGISTER_H
#define JODA_MODULEREGISTER_H

#include <joda/extension/python/PythonModule.h>
#include <joda/query/aggregation/IAggregator.h>
#include <joda/query/values/IValueProvider.h>
#include <joda/queryexecution/IQueryExecutor.h>
#include <joda/parser/IImportSource.h>
#include <joda/export/IExportDestination.h>
#include <joda/extension/ModuleType.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace joda::extension {

/**
 *  Generic Module exception
 */
class ModuleException : public std::exception {
 public:
  explicit ModuleException(const std::string &whatStr) : whatStr(whatStr) {}
  const char *what() const throw() override { return whatStr.c_str(); }

 private:
  ModuleException() = default;
  std::string whatStr;
};

class UnknownModuleType : public ModuleException {
 public:
  explicit UnknownModuleType(const std::string &path)
      : ModuleException("Unknown or unsupported module type in module '" +
                        path + "'") {}
};

class UnknownModuleLanguage : public ModuleException {
 public:
  explicit UnknownModuleLanguage(const std::string &path)
      : ModuleException("Unknown or unsupported module language in module '" +
                        path + "'") {}
};

class ModuleDoesNotExist : public ModuleException {
 public:
  explicit ModuleDoesNotExist(const std::string &path)
      : ModuleException("Could not find or access module at '" + path + "'") {}
};

struct ModuleSummary {
  std::string name;
  std::string path;
  std::string language;
  ModuleType type;
};

class ModuleRegister {
 public:
  /**
   * (Creates and) returns the global ModuleRegister instance.
   */
  static ModuleRegister &getInstance() {
    static ModuleRegister instance;  // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }

  explicit ModuleRegister();
  ~ModuleRegister();

  /**
   * Register a module at the given path
   * @param modulePath Path to the module
   * @throws UnknownModuleLanguage if the module language is unknown or
   * unsupported
   * @throws UnknownModuleType if the module type is unknown or unsupported
   */
  void registerModule(const std::string &modulePath);

  /**
   * Given a aggregation function name and parameters, creates and returns an
   * user-supplied aggregation function.
   * @param name Name of the aggregation function
   * @param toPointer Pointer where the AGG function writes the result to
   * @param params Parameters of the aggregation function
   */
  std::unique_ptr<query::IAggregator> getAggFunc(
      const std::string &funcName, const std::string &toPointer,
      std::vector<std::unique_ptr<joda::query::IValueProvider>> &&params) const;
  /**
   * Returns a list of all registered aggregation functions in user-supplied
   * modules.
   * @return List of all registered aggregation functions
   */
  std::vector<std::string> getAggFuncs() const;

  /**
   * Returns a list of all registered ValueProvider functions in user-supplied
   * modules.
   * @return List of all registered ValueProvider functions
   */
  std::vector<std::string> getIValFuncs() const;

  /**
   * Given a value provider name and parameters, creates and returns an
   * user-supplied value provider.
   * @param name Name of the value provider
   * @param params Parameters of the value provider
   */
  std::unique_ptr<query::IValueProvider> getIValFunc(
      const std::string &funcName,
      std::vector<std::unique_ptr<joda::query::IValueProvider>> &&params) const;

  /**
   * @brief Creates and returns QueryExecutors based on user supplied modules
   * 
   * @param q The query used to initialize
   * @return std::vector<std::unique_ptr<IQueryExecutor>> List of query executors
   */
  std::vector<std::unique_ptr<IQueryExecutor>> getIndices(const joda::query::Query &q) const;

  /**
   * @brief Checks wether the given module exists as an import source
   * 
   * @param moduleName the name of the module
   * @return true If it exists as
   * @return false else
   */
  bool importExists(const std::string &moduleName) const;

  /**
   * @brief Given a parameter string and sample number, an import source is created and returned
   * 
   * @param moduleName The name of the module
   * @param param The parameter string
   * @param sample a sample fraction
   * @return std::unique_ptr<docparsing::IImportSource> The import source
   */
  std::unique_ptr<docparsing::IImportSource> getImportSource(
      const std::string &moduleName,const std::string& param, double sample) const;

    /**
   * @brief Checks wether the given module exists as an export destination
   * 
   * @param moduleName the name of the module
   * @return true If it exists as
   * @return false else
   */
  bool exportExists(const std::string &moduleName) const;

    /**
   * @brief Given a parameter string, an export destination is created and returned
   * 
   * @param moduleName The name of the module
   * @param param The parameter string
   * @param sample a sample fraction
   * @return std::unique_ptr<docparsing::IExportDestination> The export destination
   */
  std::unique_ptr<IExportDestination> getExportDestination(
      const std::string &moduleName,const std::string& param) const;

  bool moduleExists(const std::string &moduleName, ModuleType type) const;

  /**
   * @brief Returns a list of all registered modules
   * 
   * @return std::vector<ModuleSummary> 
   */
  std::vector<ModuleSummary> getModules() const;

 private:
#ifdef JODA_ENABLE_PYTHON
  std::unordered_map<std::string, std::shared_ptr<python::PythonModule>>
      python_modules;
  PyThreadState *global_py_state = nullptr;
#endif 
};
}  // namespace joda::extension

#endif  // JODA_MODULEREGISTER_H
