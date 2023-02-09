#ifndef JODA_PYTHONAGGREGATOR_H
#define JODA_PYTHONAGGREGATOR_H

#ifdef JODA_ENABLE_PYTHON

#include <Python.h>
#include <joda/extension/python/PythonModule.h>
#include <joda/query/aggregation/IAggregator.h>

namespace joda::query {
class PythonAggregator : public IAggregator {
 public:
  PythonAggregator(
      const std::shared_ptr<extension::python::PythonModule> &pyModule,
      const std::string &toPointer,
      std::vector<std::unique_ptr<IValueProvider>> &&params);

  ~PythonAggregator();
  void merge(IAggregator *other) override;
  RJValue terminate(RJMemoryPoolAlloc &alloc) override;
  std::unique_ptr<IAggregator> duplicate() const override;
  void accumulate(const RapidJsonDocument &json,
                  RJMemoryPoolAlloc &alloc) override;
  const std::string getName() const override;

 private:
  PyObject *pStateOwn;


  std::shared_ptr<extension::python::PythonModule> pyModule;
};
}  // namespace joda::query

#endif  // JODA_ENABLE_PYTHON

#endif  // JODA_PYTHONAGGREGATOR_H
