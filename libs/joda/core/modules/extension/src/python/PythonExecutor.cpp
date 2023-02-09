#include <joda/extension/python/PythonExecutor.h>
#include <joda/extension/python/PythonHandler.h>
#include <joda/query/Query.h>
#ifdef JODA_ENABLE_PYTHON

// TODO Keep track of states and delete them?

namespace joda::extension::python {
PythonExecutor::PythonExecutor(
    const joda::query::Query &q,
    std::vector<std::shared_ptr<joda::extension::python::PythonModule>>
        &&indexModules)
    : pModules(std::move(indexModules)) {
  predStr = q.getChoose()->toString();
}

PythonExecutor::PythonExecutor(
    const std::string &predStr,
    const std::vector<std::shared_ptr<joda::extension::python::PythonModule>>
        &indexModules)
    : pModules(indexModules), predStr(predStr) {}

std::string PythonExecutor::getName() const {
  std::string concrete = "";
  if (chosenIndex != nullptr) {
    concrete = "(" + chosenIndex->getName() + ")";
  }
  return "PythonExecutor" + concrete;
}

std::unique_ptr<IQueryExecutor> PythonExecutor::duplicate() {
  return std::make_unique<PythonExecutor>(*this);
}

unsigned long PythonExecutor::estimatedWork(const joda::query::Query &q,
                                            JSONContainer &cont) {
  unsigned long ret = NOT_APPLICABLE;
  if (pModules.empty()) return ret;

  const auto &storage = cont.getModuleStorage();

  // Acquire GIL
  auto gstate = PyGILState_Ensure();

  // Get document count
  auto *pCountOwn = PyLong_FromLong(cont.size());
  // Get predicate string
  auto *pPredStringOwn = PyUnicode_FromString(predStr.c_str());

  // For every module get the estimated work
  for (auto &pModule : pModules) {
    try {
      // Get module state
      auto *pStateOwned = storage->getPythonStorage(pModule->getName());
      if (pStateOwned == nullptr) {
        pStateOwned = initialize_state(pModule, storage);
      }

      // Create arguments
      PyObject *pArgsOwned = PyTuple_New(3);
      // Tuple steals references, but we want to continue using them, so we have
      // to increment first
      Py_INCREF(pPredStringOwn);
      Py_INCREF(pCountOwn);
      Py_INCREF(pStateOwned);

      PyTuple_SetItem(pArgsOwned, 0, pPredStringOwn);  // Set pred string
      PyTuple_SetItem(pArgsOwned, 1, pStateOwned);     // Set state
      PyTuple_SetItem(pArgsOwned, 2, pCountOwn);       // Set doc count

      // Get the estimated work
      auto *pEstimationOwn =
          pModule->callFunction(PythonModule::INDEX_ESTIMATE_NAME, pArgsOwned);

      // Clean up arguments
      Py_CLEAR(pArgsOwned);

      // Translate estimation
      if (pEstimationOwn != Py_None) {
        unsigned long estimation = PyLong_AsUnsignedLong(pEstimationOwn);

        // Check if estimation is better than previous
        if (estimation < ret || ret == NOT_APPLICABLE) {
          // If yes, set estimation and module
          ret = estimation;
          chosenIndex = pModule;
          chosenState = pStateOwned;
        }
      }
      Py_CLEAR(pEstimationOwn);
    } catch (const extension::python::PythonModuleException &e) {
      LOG(ERROR) << e.what();
    }
  }

  // Cleanup
  Py_CLEAR(pCountOwn);
  Py_CLEAR(pPredStringOwn);

  // Release GIL
  PyGILState_Release(gstate);

  return ret;
}

std::shared_ptr<const DocIndex> PythonExecutor::execute(
    const joda::query::Query &q, JSONContainer &cont) {
  if (chosenIndex == nullptr) {
    return nullptr;
  }
  std::shared_ptr<const DocIndex> index = nullptr;

  // Acquire GIL
  auto gstate = PyGILState_Ensure();

  bool finished = false;
  try {
    // First execute state only
    index = execute_state(cont, finished);

    if (index == nullptr) {
      // Execute with documents if it returned invalid result
      index = execute_documents(cont, finished);
    }

    // Evaluate rest if unfinished
    if (!finished) {
      std::function<bool(RapidJsonDocument &, size_t)> fun =
          [&q](RapidJsonDocument &doc, size_t i) { return q.check(doc); };
      index = cont.checkDocuments(fun);
    }

  } catch (const extension::python::PythonModuleException &e) {
    LOG(ERROR) << e.what();
    // Fallback select none
    index = std::make_shared<const DocIndex>(cont.size());
  }

  // Release GIL
  PyGILState_Release(gstate);

  return index;
}

std::shared_ptr<const DocIndex> PythonExecutor::execute_state(
    JSONContainer &cont, bool &finished) const {
  // Get predicate string
  auto *pPredStringOwn = PyUnicode_FromString(predStr.c_str());
  // Create arguments
  PyObject *pArgsOwned = PyTuple_New(2);
  // Increase state counter as it will be stolen from argument
  Py_INCREF(chosenState);
  // Set arguments
  PyTuple_SetItem(pArgsOwned, 0, pPredStringOwn);  // Set pred string
  PyTuple_SetItem(pArgsOwned, 1, chosenState);     // Set state

  PyObject *result;
  // Get state execution result
  try {
    result = chosenIndex->callFunction(PythonModule::INDEX_EXECUTE_STATE_NAME,
                                       pArgsOwned);
  } catch (const extension::python::PythonModuleException &e) {
    // Cleanup
    Py_CLEAR(pArgsOwned);
    throw;  // Pass on exception
  }
  // Cleanup
  Py_CLEAR(pArgsOwned);

  if (result == nullptr || result == Py_None) {
    // Cleanup
    Py_CLEAR(result);
    return nullptr;
  }

  PyObject *pIndexBorrow, *pFinishedFlagBorrow;
  // Get result tuple contents
  pIndexBorrow = PyTuple_GetItem(result, 0);
  pFinishedFlagBorrow = PyTuple_GetItem(result, 1);

  // Set flag
  finished = PyObject_IsTrue(pFinishedFlagBorrow) != 0;

  // Translate to C++
  auto index = translate_index(pIndexBorrow, cont.size());

  // Cleanup
  Py_CLEAR(result);
  return index;
}

std::shared_ptr<const DocIndex> PythonExecutor::execute_documents(
    JSONContainer &cont, bool &finished) const {
  // Get predicate string
  auto *pPredStringOwn = PyUnicode_FromString(predStr.c_str());

  // Translate documents
  std::vector<PyObject *> pDocsOwned;
  std::function<PyObject *(RapidJsonDocument &)> fun =
      [](RapidJsonDocument &doc) {
        python::handler::PyHandler handler;
        doc.Accept(handler);
        return handler.root;
      };
  cont.forAll(fun, pDocsOwned);
  // Create list
  auto *pDocListOwned = PyList_New(pDocsOwned.size());
  // Fill list
  for (size_t i = 0; i < pDocsOwned.size(); i++) {
    PyList_SetItem(pDocListOwned, i, pDocsOwned[i]);
  }

  // Create arguments
  PyObject *pArgsOwned = PyTuple_New(3);
  // Increase state counter as it will be stolen from argument
  Py_INCREF(chosenState);
  // Set arguments
  PyTuple_SetItem(pArgsOwned, 0, pPredStringOwn);  // Set pred string
  PyTuple_SetItem(pArgsOwned, 1, chosenState);     // Set state
  PyTuple_SetItem(pArgsOwned, 2, pDocListOwned);   // Set list

  PyObject *result;
  // Get state execution result
  try {
    result = chosenIndex->callFunction(PythonModule::INDEX_EXECUTE_DOCS_NAME,
                                       pArgsOwned);
  } catch (const extension::python::PythonModuleException &e) {
    // Cleanup
    Py_CLEAR(pArgsOwned);
    throw;  // Pass on exception
  }
  // Cleanup
  Py_CLEAR(pArgsOwned);

  if (result == nullptr || result == Py_None) {
    // Cleanup
    Py_CLEAR(result);
    return nullptr;
  }

  PyObject *pIndexBorrow, *pFinishedFlagBorrow;
  // Get result tuple contents
  pIndexBorrow = PyTuple_GetItem(result, 0);
  pFinishedFlagBorrow = PyTuple_GetItem(result, 1);

  // Set flag
  finished = PyObject_IsTrue(pFinishedFlagBorrow) != 0;

  // Translate to C++
  auto index = translate_index(pIndexBorrow, cont.size());

  // Cleanup
  Py_CLEAR(result);
  return index;
}

void PythonExecutor::alwaysAfterSelect(const joda::query::Query &q,
                                       std::shared_ptr<const DocIndex> &sel,
                                       JSONContainer &cont) {
  if (pModules.empty()) return;

  const auto &storage = cont.getModuleStorage();

  // Acquire GIL
  auto gstate = PyGILState_Ensure();

  // Translate index
  auto *pIndexOwn = translate_index(*sel);
  // Get predicate string
  auto *pPredStringOwn = PyUnicode_FromString(predStr.c_str());

  // For every module get the estimated work
  for (auto &pModule : pModules) {
    try {
      // Get module state
      auto *pStateOwned = storage->getPythonStorage(pModule->getName());
      if (pStateOwned == nullptr) {
        pStateOwned = initialize_state(pModule, storage);
      }

      // Create arguments
      PyObject *pArgsOwned = PyTuple_New(3);
      // Tuple steals references, but we want to continue using them, so we have
      // to increment first
      Py_INCREF(pPredStringOwn);
      Py_INCREF(pIndexOwn);
      Py_INCREF(pStateOwned);

      PyTuple_SetItem(pArgsOwned, 0, pPredStringOwn);  // Set pred string
      PyTuple_SetItem(pArgsOwned, 1, pStateOwned);     // Set state
      PyTuple_SetItem(pArgsOwned, 2, pIndexOwn);       // Set index

      // Get the estimated work
      auto *res =
          pModule->callFunction(PythonModule::INDEX_IMPROVE_NAME, pArgsOwned);

      // Clean up arguments
      Py_CLEAR(pArgsOwned);
      Py_CLEAR(res);

    } catch (const extension::python::PythonModuleException &e) {
      LOG(ERROR) << e.what();
    }
  }

  // Cleanup
  Py_CLEAR(pIndexOwn);
  Py_CLEAR(pPredStringOwn);

  // Release GIL
  PyGILState_Release(gstate);
}

PyObject *PythonExecutor::initialize_state(
    std::shared_ptr<joda::extension::python::PythonModule> &pModule,
    const std::unique_ptr<joda::extension::ModuleExecutorStorage> &storage) {
  auto *stateOwned = pModule->callFunction(
      extension::python::PythonModule::INDEX_INIT_NAME, nullptr);
  storage->setPythonStorage(pModule->getName(), stateOwned);

  return stateOwned;
}

std::shared_ptr<const DocIndex> PythonExecutor::translate_index(
    PyObject *oBorrow, unsigned long size) {
  if (oBorrow == nullptr || oBorrow == Py_None || PyList_Check(oBorrow) == 0) {
    return nullptr;
  }

  DocIndex index(size, false);

  PyObject *iterator = PyObject_GetIter(oBorrow);
  PyObject *item;
  size_t i = 0;

  if (iterator == nullptr) {
    return nullptr;
  }

  while ((item = PyIter_Next(iterator))) {
    // Fetch bool
    if (item == Py_True) {
      index[i] = true;
    }
    i++;
    /* release reference when done */
    Py_DECREF(item);
  }

  Py_DECREF(iterator);

  return std::make_shared<DocIndex>(std::move(index));
}

PyObject *PythonExecutor::translate_index(const DocIndex &index) {
  auto *pListOwned = PyList_New(index.size());
  for (size_t i = 0; i < index.size(); i++) {
    if (index[i]) {
      PyList_SetItem(pListOwned, i, Py_True);
      Py_IncRef(Py_True);
    } else {
      PyList_SetItem(pListOwned, i, Py_False);
      Py_IncRef(Py_False);
    }
  }
  return pListOwned;
}

}  // namespace joda::extension::python

#endif  // JODA_ENABLE_PYTHON