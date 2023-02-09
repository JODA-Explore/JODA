#ifndef JODA_PYTHONHANDLER_H
#define JODA_PYTHONHANDLER_H

#ifdef JODA_ENABLE_PYTHON

#include <Python.h>
#include <joda/misc/RJFwd.h>

#include <vector>

namespace joda::extension::python::handler {

// Adapted from
// https://github.com/python-rapidjson/python-rapidjson/blob/master/rapidjson.cpp
// On 16.05.2022
// Original Notice:
// :Project:   python-rapidjson -- Python extension module
// :Author:    Ken Robbins <ken@kenrobbins.com>
// :License:   MIT License
// :Copyright: © 2015 Ken Robbins
// :Copyright: © 2015, 2016, 2017, 2018, 2019, 2020, 2021, 2022 Lele Gaifax
//

struct HandlerContext {
  PyObject* object;
  const char* key;
  rapidjson::SizeType keyLength;
  bool isObject;
  bool keyValuePairs;
  bool copiedKey;
};

struct PyHandler {
  PyObject* sharedKeys;
  PyObject* root = nullptr;
  std::vector<HandlerContext> stack;

  PyHandler() {
    stack.reserve(128);
    sharedKeys = PyDict_New();
  }

  PyHandler(const PyHandler&) = delete;
  PyHandler& operator=(const PyHandler&) = delete;

  ~PyHandler() {
    while (!stack.empty()) {
      const HandlerContext& ctx = stack.back();
      if (ctx.copiedKey) PyMem_Free((void*)ctx.key);
      if (ctx.object != nullptr) Py_DECREF(ctx.object);
      stack.pop_back();
    }
    Py_CLEAR(sharedKeys);
  }

  bool Handle(PyObject* value) {
    if (root) {
      const HandlerContext& current = stack.back();

      if (current.isObject) {
        PyObject* key =
            PyUnicode_FromStringAndSize(current.key, current.keyLength);
        if (key == nullptr) {
          Py_DECREF(value);
          return false;
        }

        PyObject* shared_key = PyDict_SetDefault(sharedKeys, key, key);
        if (shared_key == nullptr) {
          Py_DECREF(key);
          Py_DECREF(value);
          return false;
        }
        Py_INCREF(shared_key);
        Py_DECREF(key);
        key = shared_key;

        int rc;
        if (current.keyValuePairs) {
          PyObject* pair = PyTuple_Pack(2, key, value);

          Py_DECREF(key);
          Py_DECREF(value);
          if (pair == nullptr) {
            return false;
          }
          rc = PyList_Append(current.object, pair);
          Py_DECREF(pair);
        } else {
          if (PyDict_CheckExact(current.object))
            // If it's a standard dictionary, this is +20% faster
            rc = PyDict_SetItem(current.object, key, value);
          else
            rc = PyObject_SetItem(current.object, key, value);
          Py_DECREF(key);
          Py_DECREF(value);
        }

        if (rc == -1) {
          return false;
        }
      } else {
        PyList_Append(current.object, value);
        Py_DECREF(value);
      }
    } else {
      root = value;
    }
    return true;
  }

  bool Key(const char* str, rapidjson::SizeType length, bool copy) {
    HandlerContext& current = stack.back();

    // This happens when operating in stream mode and kParseInsituFlag is not
    // set: we must copy the incoming string in the context, and destroy the
    // duplicate when the context gets reused for the next dictionary key

    if (current.key && current.copiedKey) {
      PyMem_Free((void*)current.key);
      current.key = nullptr;
    }

    if (copy) {
      char* copied_str = (char*)PyMem_Malloc(length + 1);
      if (copied_str == nullptr) return false;
      memcpy(copied_str, str, length + 1);
      str = copied_str;
      assert(!current.key);
    }

    current.key = str;
    current.keyLength = length;
    current.copiedKey = copy;

    return true;
  }

  bool StartObject() {
    PyObject* mapping;
    bool key_value_pairs;

    mapping = PyDict_New();
    if (mapping == nullptr) {
      return false;
    }
    key_value_pairs = false;

    if (!Handle(mapping)) {
      return false;
    }

    HandlerContext ctx;
    ctx.isObject = true;
    ctx.keyValuePairs = key_value_pairs;
    ctx.object = mapping;
    ctx.key = nullptr;
    ctx.copiedKey = false;
    Py_INCREF(mapping);

    stack.push_back(ctx);

    return true;
  }

  bool EndObject(rapidjson::SizeType member_count) {
    const HandlerContext& ctx = stack.back();

    if (ctx.copiedKey) PyMem_Free((void*)ctx.key);

    PyObject* mapping = ctx.object;
    stack.pop_back();

    Py_DECREF(mapping);
    return true;
  }

  bool StartArray() {
    PyObject* list = PyList_New(0);
    if (list == nullptr) {
      return false;
    }

    if (!Handle(list)) {
      return false;
    }

    HandlerContext ctx;
    ctx.isObject = false;
    ctx.object = list;
    ctx.key = nullptr;
    ctx.copiedKey = false;
    Py_INCREF(list);

    stack.push_back(ctx);

    return true;
  }

  bool EndArray(rapidjson::SizeType elementCount) {
    const HandlerContext& ctx = stack.back();

    if (ctx.copiedKey) PyMem_Free((void*)ctx.key);

    PyObject* sequence = ctx.object;
    stack.pop_back();

    Py_DECREF(sequence);
    return true;
  }

  bool Null() {
    PyObject* value = Py_None;
    Py_INCREF(value);

    return Handle(value);
  }

  bool Bool(bool b) {
    PyObject* value = b ? Py_True : Py_False;
    Py_INCREF(value);

    return Handle(value);
  }

  bool Int(int i) {
    PyObject* value = PyLong_FromLong(i);
    return Handle(value);
  }

  bool Uint(unsigned i) {
    PyObject* value = PyLong_FromUnsignedLong(i);
    return Handle(value);
  }

  bool Int64(int64_t i) {
    PyObject* value = PyLong_FromLongLong(i);
    return Handle(value);
  }

  bool Uint64(uint64_t i) {
    PyObject* value = PyLong_FromUnsignedLongLong(i);
    return Handle(value);
  }

  bool Double(double d) {
    PyObject* value = PyFloat_FromDouble(d);
    return Handle(value);
  }

  bool RawNumber(const char* str, rapidjson::SizeType length, bool copy) const {
    return true;
  }

  bool String(const char* str, rapidjson::SizeType length, bool copy) {
    PyObject* value = nullptr;

    value = PyUnicode_FromStringAndSize(str, length);
    if (value == nullptr) return false;

    return Handle(value);
  }
};

}  // namespace joda::extension::python::handler

#endif  // JODA_ENABLE_PYTHON

#endif  // JODA_PYTHONHANDLER_H