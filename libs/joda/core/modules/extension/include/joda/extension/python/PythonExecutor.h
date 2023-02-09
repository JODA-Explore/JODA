#ifndef JODA_PYTHONEXECUTOR_H
#define JODA_PYTHONEXECUTOR_H

#ifdef JODA_ENABLE_PYTHON

#include <Python.h>
#include <joda/extension/python/PythonModule.h>
#include <joda/queryexecution/IQueryExecutor.h>

namespace joda::extension::python {

class PythonExecutor : public IQueryExecutor {
public:
  explicit PythonExecutor(
      const joda::query::Query &q,
      std::vector<std::shared_ptr<joda::extension::python::PythonModule>>
          &&indexModules);
  unsigned long estimatedWork(const joda::query::Query &q,
                              JSONContainer &cont) override;
  std::shared_ptr<const DocIndex> execute(const joda::query::Query &q,
                                          JSONContainer &cont) override;
  std::string getName() const override;
  void alwaysAfterSelect(const joda::query::Query &q,
                         std::shared_ptr<const DocIndex> &sel,
                         JSONContainer &cont) override;
  std::unique_ptr<IQueryExecutor> duplicate() override;

 private:
  std::vector<std::shared_ptr<joda::extension::python::PythonModule>> pModules;
  std::string predStr;

  std::shared_ptr<joda::extension::python::PythonModule> chosenIndex = nullptr;
  PyObject* chosenState = nullptr;

  std::shared_ptr<const DocIndex> execute_state(JSONContainer &cont, bool& finished) const;
  std::shared_ptr<const DocIndex> execute_documents(JSONContainer &cont, bool& finished) const;

  static std::shared_ptr<const DocIndex> translate_index(PyObject* oBorrow, unsigned long size);
  static PyObject* translate_index(const DocIndex& index);

  explicit PythonExecutor(
      const std::string& predStr,
      const std::vector<std::shared_ptr<joda::extension::python::PythonModule>>
          &indexModules);

  PyObject *initialize_state(
      std::shared_ptr<joda::extension::python::PythonModule> &pModule,
      const std::unique_ptr<joda::extension::ModuleExecutorStorage> &storage);
};

}  // namespace joda::extension::python

#endif  // JODA_ENABLE_PYTHON

#endif  // JODA_PYTHONEXECUTOR_H
