#ifndef JODA_PYTHONGENERATOR_H
#define JODA_PYTHONGENERATOR_H

#ifdef JODA_ENABLE_PYTHON

#include <Python.h>
#include <joda/misc/RJFwd.h>

#include <vector>

namespace joda::extension::python::handler {

class PythonGenerator {
 public:
  PythonGenerator(PyObject* root) : root(root) {}

  template <typename Handler>
  bool Accept(Handler& handler, PyObject* obj) const {
    if (obj == nullptr || obj == Py_None) {
      return handler.Null();
    }

    // Bool
    if (PyBool_Check(obj)) {
      if (PyObject_IsTrue(obj)) return handler.Bool(true);
      return handler.Bool(false);
    }

    // Int
    if (PyLong_Check(obj)) {
      return handler.Int64(PyLong_AsLong(obj));
    }
    // Float
    if (PyFloat_Check(obj)) {
      return handler.Double(PyFloat_AsDouble(obj));
    }
    // String
    if (PyUnicode_Check(obj)) {
      const auto* s = PyUnicode_AsUTF8(obj);
      return handler.String(s, strlen(s), true);
    }

    // List
    if (PyList_Check(obj)) {
      auto start = handler.StartArray();
      if (!start) return false;
      auto size = PyList_Size(obj);
      for (Py_ssize_t i = 0; i < PyList_Size(obj); i++) {
        auto* item = PyList_GetItem(obj, i);
        auto item_ret = Accept(handler, item);
        if (!item_ret) return false;
      }

      return handler.EndArray(size);
    }

    // Dict
    if (PyDict_Check(obj)) {
      auto start = handler.StartObject();
      if (!start) return false;
      PyObject *key, *value;
      Py_ssize_t pos = 0;

      size_t count = 0;
      while (PyDict_Next(obj, &pos, &key, &value)) {
        if (!PyUnicode_Check(key)) {
          return false;
        }
        const auto* s = PyUnicode_AsUTF8(key);
        auto key_ret = handler.String(s, strlen(s), true);
        if (!key_ret) return false;
        auto value_ret = Accept(handler, value);
        if (!value_ret) return false;
        count++;
      }
      return handler.EndObject(count);
    }

    const auto* pyType = Py_TYPE(obj);
    std::string typeName = pyType->tp_name;

    LOG(WARNING) << "Unsupported type '" << typeName
                  << "' in Python => JSON conversion";
    return false;
  }

  template <typename Handler>
  bool Accept(Handler& handler) const {
    return Accept(handler, root);
  }

  // Implement generator
  template <typename Handler>
  bool operator()(Handler& handler) const {
    return Accept(handler);
  }

 private:
  PyObject* root;
};

}  // namespace joda::extension::python::handler

#endif  // JODA_ENABLE_PYTHON

#endif  // JODA_PYTHONGENERATOR_H