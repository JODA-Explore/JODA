#ifndef JODA_PYTHONMODULE_H
#define JODA_PYTHONMODULE_H

#ifdef JODA_ENABLE_PYTHON

#include <Python.h>
#include <joda/extension/ModuleType.h>
#include <joda/misc/RJFwd.h>
#include <joda/query/aggregation/IAggregator.h>
#include <joda/query/values/IValueProvider.h>

#include <string>

namespace joda::extension::python {

/**
 *  Generic Python Module exception
 */
class PythonModuleException : public std::exception {
 public:
  explicit PythonModuleException(const std::string &whatStr)
      : whatStr(whatStr) {}
  const char *what() const throw() override { return whatStr.c_str(); }

 private:
  PythonModuleException() = default;
  std::string whatStr;
};

class PythonModuleMissing : public PythonModuleException {
 public:
  explicit PythonModuleMissing(const std::string &name)
      : PythonModuleException("Can't find/load python module '" + name + "'") {}
  explicit PythonModuleMissing(const std::string &name,
                               const std::string &reason)
      : PythonModuleException("Can't find/load python module '" + name +
                              "': " + reason) {}
};

class PythonModuleDictException : public PythonModuleException {
 public:
  explicit PythonModuleDictException()
      : PythonModuleException("Can't access the dict of the module") {}
};

class PythonModuleMissingFunction : public PythonModuleException {
 public:
  explicit PythonModuleMissingFunction(const std::string &module_name,
                                       const std::string &name)
      : PythonModuleException("Missing function '" + name +
                              "' in python module '" + module_name + "'") {}
};

class PythonModuleFunctionError : public PythonModuleException {
 public:
  explicit PythonModuleFunctionError(const std::string &name)
      : PythonModuleException("Could not execute function '" + name + "'") {}
  explicit PythonModuleFunctionError(const std::string &name,
                                     const std::string &reason)
      : PythonModuleException("Could not execute function '" + name +
                              "': " + reason) {}
};

class PythonModuleMissingVariable : public PythonModuleException {
 public:
  explicit PythonModuleMissingVariable(const std::string &module_name,
                                       const std::string &name)
      : PythonModuleException("Missing variable '" + name +
                              "' in python module '" + module_name + "'") {}
};

class PythonModule {
 public:
  // Shared
  static constexpr auto NUM_ARGS_NAME = "num_args";
  // Aggregation
  static constexpr auto AGG_FUNC_NAME = "aggregate";
  static constexpr auto AGG_FINAL_NAME = "finalize";
  static constexpr auto AGG_MERGE_NAME = "merge";
  static constexpr auto AGG_INIT_NAME = "init_state";
  // Value Provider
  static constexpr auto IVAL_GETVALUE_NAME = "get_value";
  // Index
  static constexpr auto INDEX_INIT_NAME = "init_index";
  static constexpr auto INDEX_ESTIMATE_NAME = "estimate_usage";
  static constexpr auto INDEX_EXECUTE_STATE_NAME = "execute_state";
  static constexpr auto INDEX_EXECUTE_DOCS_NAME = "execute_docs";
  static constexpr auto INDEX_IMPROVE_NAME = "improve_index";
  // Import
  static constexpr auto IMPORT_GET_NEXT_NAME = "get_next";
  static constexpr auto IMPORT_INIT_NAME = "init";
  // Export
  static constexpr auto EXPORT_SET_NEXT_NAME = "set_next";
  static constexpr auto EXPORT_INIT_NAME = "init";
  static constexpr auto EXPORT_FINALIZE_NAME = "finalize";

  /**
   * @brief Initialize Python module at the given path
   * @param modulePath Path to the Python module
   */
  explicit PythonModule(const std::string_view &modulePath);

  ~PythonModule();

  /**
   * Returns the name of the underlying module
   */
  std::string getName() const;

  /**
   * Returns the path of the underlying module
   */
  std::string getPath() const;

  /**
   * Returns the type of the underlying module
   */
  ModuleType getType() const;

  /**
   * Returns the number of arguments the function takes
   */
  size_t getNumParams() const;

  /**
   * Returns an BORROWED variable from the module dict
   */
  PyObject *getVar(const std::string &name);

  /**
   * This function BORROWS the argument, uses it to call the function, and
   * returns a OWNED result
   */
  PyObject *callFunction(const std::string &name, PyObject *pArgsBorrow);

  /**
   * Converts a given PyObject to a RJValue
   * @param pObj PyObject to convert
   * @return Converted RJValue
   */
  RJValue convert(PyObject *pObjBorrow, RJMemoryPoolAlloc &alloc) const;
  
  /**
   * @brief Getrs the return type of the get_value function
   * 
   * @return query::IValueType 
   * 
   */
  query::IValueType getReturnType() const;


 private:
  std::string module_path;
  std::string module_name;
  ModuleType module_type = ModuleType::UNKNOWN;
  size_t num_params = 0;
  query::IValueType ret_type = query::IValueType::IV_Any;
  PyObject *pModule = nullptr;

  size_t getNumParams(PyObject *dict) const;
  query::IValueType getRetType(PyObject *dict) const;
  /**
   * @brief Validates the Python module
   */
  void validate();

  /**
   * @brief Checks if the given function exists and is callable.
   * @param name Name of the function
   * @param dict The module dictionary
   * @return True if the function exists and is callable
   */
  bool validateFunction(const std::string &name, PyObject *dict) const;

  void ensureVar(const std::string &name, PyObject *dict) const;

  query::IValueType getRetType();

  /**
   * Returns an BORROWED dictionary of the module content
   */
  PyObject *getModuleDict();
};

[[maybe_unused]] static inline std::string GetPythonError() {
  PyObject *ptype = nullptr, *pvalue = nullptr, *ptraceback = nullptr;
  PyErr_Fetch(&ptype, &pvalue, &ptraceback);
  PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
  std::string message("");
  if (pvalue) {
    auto *pMsg = PyObject_Str(pvalue);
    message = PyUnicode_AsUTF8(pMsg);
    Py_CLEAR(pMsg);
  }
  Py_XDECREF(ptraceback);
  Py_XDECREF(pvalue);
  Py_XDECREF(ptype);
  return message;
}

}  // namespace joda::extension::python

#endif  // JODA_ENABLE_PYTHON

#endif  // JODA_PYTHONMODULE_H
