#ifndef JODA_PYTHONVALUE_H
#define JODA_PYTHONVALUE_H

#ifdef JODA_ENABLE_PYTHON

#include <Python.h>
#include <joda/extension/python/PythonModule.h>
#include <joda/query/values/IValueProvider.h>

namespace joda::query {

class PythonValueProvider : public joda::query::IValueProvider {
 public:
  explicit PythonValueProvider(const std::shared_ptr<extension::python::PythonModule>& pyModule,
      std::vector<std::unique_ptr<IValueProvider>> &&parameters);
  std::unique_ptr<IValueProvider> duplicate() const override;
  bool isConst() const override;
  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;

  bool isAtom() const override;

  std::string getName() const override;
  IValueType getReturnType() const override;

 private:
  std::shared_ptr<extension::python::PythonModule> pyModule;
  IValueType returnType = IValueType::IV_Any;
};

}  // namespace joda::query

#endif  // JODA_ENABLE_PYTHON

#endif  // JODA_PYTHONVALUE_H
