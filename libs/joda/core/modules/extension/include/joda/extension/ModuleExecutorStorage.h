#ifndef JODA_MODULEExecutorStorage_H
#define JODA_MODULEExecutorStorage_H

#include <string>
#include <unordered_map>
#ifdef JODA_ENABLE_PYTHON
#include <Python.h>
#endif

namespace joda::extension {

/**
 * @brief Stores the index data of the external module indices
 * 
 */
class ModuleExecutorStorage {
 public:
  ~ModuleExecutorStorage();

#ifdef JODA_ENABLE_PYTHON
  /**
   * @brief Searches and returns the storage of the given python module.
   * 
   * @param name The name of the python module
   * @return PyObject* The storage of the python module or a nullptr if it does not exist
   */
  PyObject* getPythonStorage(const std::string& name);

  /**
   * @brief Sets the python state of the index
   * 
   * @param name The name of the module
   * @param stateBorrowed The state of the index. The state is owned by the caller
   */
  void setPythonStorage(const std::string& name, PyObject* stateBorrowed);
#endif

 private:
#ifdef JODA_ENABLE_PYTHON
  std::unordered_map<std::string, PyObject*> pythonStorage;
#endif
};

}  // namespace joda::extension

#endif  // JODA_MODULEExecutorStorage_H
