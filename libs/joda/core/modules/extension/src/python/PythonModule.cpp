#include <glog/logging.h>
#include <joda/extension/python/PythonAggregator.h>
#include <joda/extension/python/PythonModule.h>

#include <filesystem>
#include <optional>
#include <sstream>

#ifdef JODA_ENABLE_PYTHON

namespace joda::extension::python {
PythonModule::PythonModule(const std::string_view &modulePath)
    : module_path(modulePath) {
  // Extract name
  std::filesystem::path path(modulePath);
  module_name = path.stem().string();
  // and directory
  auto module_dir = path.parent_path();

  // Initiate python import statement
  std::ostringstream python_path;
  python_path << "sys.path.append(\"" << module_dir.c_str() << "\")";

  // Execute python import statement
  PyRun_SimpleString("import sys");
  PyRun_SimpleString(python_path.str().c_str());

  validate();
}

PythonModule::~PythonModule() {
  auto gstate = PyGILState_Ensure();
  Py_CLEAR(pModule);
  PyGILState_Release(gstate);
}

ModuleType PythonModule::getType() const { return module_type; }

void PythonModule::validate() {
  auto *pDictBorrow = getModuleDict();

  // Infer module type
  // - Try aggregate
  if (validateFunction(AGG_FUNC_NAME, pDictBorrow) &&
      validateFunction(AGG_MERGE_NAME, pDictBorrow) &&
      validateFunction(AGG_FINAL_NAME, pDictBorrow) &&
      validateFunction(AGG_INIT_NAME, pDictBorrow)) {
    // Get number of function parameters
    num_params = getNumParams(pDictBorrow);

    LOG(INFO) << "Initialized python module '" << module_name
              << "' as AGGREGATE";
    // Set module type
    module_type = ModuleType::AGG;
  }
  // - Try ValueProvider
  if (validateFunction(IVAL_GETVALUE_NAME, pDictBorrow)) {
    // Get number of function parameters
    num_params = getNumParams(pDictBorrow);
    // Get function return type
    ret_type = getRetType(pDictBorrow);

    LOG(INFO) << "Initialized python module '" << module_name
              << "' as VALUEPROVIDER";

    // Set module type
    module_type = ModuleType::IVAL;


  }
  // - Try index
  if (validateFunction(INDEX_ESTIMATE_NAME, pDictBorrow) &&
      validateFunction(INDEX_EXECUTE_DOCS_NAME, pDictBorrow) &&
      validateFunction(INDEX_EXECUTE_STATE_NAME, pDictBorrow) &&
      validateFunction(INDEX_IMPROVE_NAME, pDictBorrow) &&
      validateFunction(INDEX_INIT_NAME, pDictBorrow)) {
    LOG(INFO) << "Initialized python module '" << module_name << "' as INDEX";
    // Set module type
    module_type = ModuleType::INDEX;
  }
  // - Try import
  if (validateFunction(IMPORT_GET_NEXT_NAME, pDictBorrow) &&
      validateFunction(IMPORT_INIT_NAME, pDictBorrow)) {
    LOG(INFO) << "Initialized python module '" << module_name << "' as IMPORT";
    // Set module type
    module_type = ModuleType::IMPORT;
  }

  // - Try Export
  if (validateFunction(EXPORT_FINALIZE_NAME, pDictBorrow) &&
      validateFunction(EXPORT_INIT_NAME, pDictBorrow) &&
      validateFunction(EXPORT_SET_NEXT_NAME, pDictBorrow)) {
    LOG(INFO) << "Initialized python module '" << module_name << "' as EXPORT";
    // Set module type
    module_type = ModuleType::EXPORT;
  }
}

bool PythonModule::validateFunction(const std::string &name,
                                    PyObject *dict) const {
  if (PyObject *pFuncBorrow = PyDict_GetItemString(dict, name.c_str());
      pFuncBorrow != nullptr && PyCallable_Check(pFuncBorrow)) {
    return true;
  }
  return false;
}

size_t PythonModule::getNumParams(PyObject *dict) const {
  if (PyObject *pArgnumBorrow = PyDict_GetItemString(dict, "num_args");
      pArgnumBorrow != nullptr) {
    if (!PyLong_Check(pArgnumBorrow)) {
      throw PythonModuleMissingVariable(module_name, "num_args");
    }
    auto num_args = PyLong_AsLong(pArgnumBorrow);
    return num_args;
  }
  throw PythonModuleMissingVariable(module_name, "num_args");
}

query::IValueType PythonModule::getRetType(PyObject *dict) const {
  if (PyObject *pArgnumBorrow = PyDict_GetItemString(dict, "ret_type");
      pArgnumBorrow != nullptr) {
    if (PyUnicode_Check(pArgnumBorrow)) {
      auto ret_type = PyUnicode_AsUTF8(pArgnumBorrow);
      if (strcmp(ret_type, "STRING") == 0) {
        return query::IV_String;
      } else if (strcmp(ret_type, "NUMBER") == 0) {
        return query::IV_Number;
      } else if (strcmp(ret_type, "BOOLEAN") == 0) {
        return query::IV_Bool;
      } else if (strcmp(ret_type, "OBJECT") == 0) {
        return query::IV_Object;
      } else if (strcmp(ret_type, "ARRAY") == 0) {
        return query::IV_Array;
      } else if (strcmp(ret_type, "ANY") == 0) {
        return query::IV_Any;
      }
    }
  }
  return query::IV_Any;
}

size_t PythonModule::getNumParams() const { return num_params; }
query::IValueType PythonModule::getReturnType() const { return ret_type; }

void PythonModule::ensureVar(const std::string &name, PyObject *dict) const {
  if (PyObject *pVarBorrow = PyDict_GetItemString(dict, name.c_str());
      pVarBorrow == nullptr) {
    throw PythonModuleMissingVariable(module_name, name);
  }
}

PyObject *PythonModule::getVar(const std::string &name) {
  // Get dict
  auto *pDictBorrow = getModuleDict();

  // Search var
  if (auto *pVarBorrow = PyDict_GetItemString(pDictBorrow, name.c_str());
      pVarBorrow != nullptr) {
    return pVarBorrow;
  }
  // Throw if not found
  throw PythonModuleMissingVariable(module_name, name);
}


PyObject *PythonModule::callFunction(const std::string &name,
                                     PyObject *pArgsBorrow) {
  // Get dict
  auto *pDictBorrow = getModuleDict();

  // Search function name
  auto *pFuncBorrow = PyDict_GetItemString(pDictBorrow, name.c_str());

  if (pFuncBorrow == nullptr) {
    throw PythonModuleMissingFunction(module_name, name);
  }

  if (!PyCallable_Check(pFuncBorrow)) {
    throw PythonModuleMissingFunction(module_name, name);
  }

  auto pResultOwn = PyObject_CallObject(pFuncBorrow, pArgsBorrow);
  if (pResultOwn == nullptr) {
    if (PyErr_Occurred()) {
      throw PythonModuleFunctionError(name, GetPythonError());
    } else {
      throw PythonModuleFunctionError(name);
    }
  }

  return pResultOwn;
}

RJValue PythonModule::convert(PyObject *pObjBorrow,
                              RJMemoryPoolAlloc &alloc) const {
  if (pObjBorrow == nullptr || pObjBorrow == Py_None) {
    return RJValue();
  }

  // Bool
  if (PyBool_Check(pObjBorrow)) {
    if (PyObject_IsTrue(pObjBorrow)) return RJValue(rapidjson::kTrueType);
    return RJValue(rapidjson::kFalseType);
  }

  // Int
  if (PyLong_Check(pObjBorrow)) {
    return RJValue(PyLong_AsLong(pObjBorrow));
  }
  // Float
  if (PyFloat_Check(pObjBorrow)) {
    return RJValue(PyFloat_AsDouble(pObjBorrow));
  }
  // String
  if (PyUnicode_Check(pObjBorrow)) {
    return RJValue(PyUnicode_AsUTF8(pObjBorrow), alloc);
  }

  // List
  if (PyList_Check(pObjBorrow)) {
    auto list = RJValue(rapidjson::kArrayType);
    for (Py_ssize_t i = 0; i < PyList_Size(pObjBorrow); i++) {
      auto *item = PyList_GetItem(pObjBorrow, i);
      list.PushBack(convert(item, alloc), alloc);
    }
    return list;
  }

  // Dict
  if (PyDict_Check(pObjBorrow)) {
    auto dict = RJValue(rapidjson::kObjectType);
    PyObject *key, *value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(pObjBorrow, &pos, &key, &value)) {
      dict.AddMember(convert(key, alloc), convert(value, alloc), alloc);
    }
    return dict;
  }
  const auto *pyType = Py_TYPE(pObjBorrow);
  std::string typeName = pyType->tp_name;

  DCHECK(false) << "Unsupported type '" << typeName
                << "' in Python => JSON conversion";
  return RJValue();
}

std::string PythonModule::getName() const { return module_name; }
std::string PythonModule::getPath() const { return module_path; }

PyObject *PythonModule::getModuleDict() {
  if (pModule == nullptr) {
    // Import module
    // Get module name (OWNED)
    auto *pModuleNameOwn = PyUnicode_FromString(module_name.c_str());

    // Import Module (OWNED)
    auto *pModuleOwn = PyImport_Import(pModuleNameOwn);
    Py_DECREF(pModuleNameOwn);
    if (pModuleOwn == nullptr) {
      if (PyErr_Occurred()) {
        throw PythonModuleMissing(module_name, GetPythonError());
      } else {
        throw PythonModuleMissing(module_name);
      }
    }
    pModule = pModuleOwn;
  }
  // Get Dict
  auto *pDictBorrow = PyModule_GetDict(pModule);
  if (pDictBorrow == nullptr) {
    throw PythonModuleDictException();
  }
  return pDictBorrow;
}


}  // namespace joda::extension::python

#endif  // JODA_ENABLE_PYTHON
