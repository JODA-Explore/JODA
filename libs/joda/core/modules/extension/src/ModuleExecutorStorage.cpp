#include <joda/extension/ModuleExecutorStorage.h>

namespace joda::extension {

ModuleExecutorStorage::~ModuleExecutorStorage() {
#ifdef JODA_ENABLE_PYTHON
  if (!pythonStorage.empty()) {
    auto gstate = PyGILState_Ensure();
    for (auto& [key, value] : pythonStorage) {
      Py_CLEAR(value);
    }
    PyGILState_Release(gstate);
  }

#endif
}

#ifdef JODA_ENABLE_PYTHON

PyObject* ModuleExecutorStorage::getPythonStorage(const std::string& name) {
  PyObject* ret = nullptr;
  if (const auto& f = pythonStorage.find(name); f != pythonStorage.end()) {
    ret = f->second;
  }
  return ret;
}

void ModuleExecutorStorage::setPythonStorage(const std::string& name,
                                             PyObject* stateBorrowed) {
  pythonStorage[name] = stateBorrowed;
}

#endif

}  // namespace joda::extension