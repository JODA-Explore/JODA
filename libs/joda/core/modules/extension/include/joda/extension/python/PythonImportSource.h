#ifndef JODA_PYTHONIMPORTSOURCE_H
#define JODA_PYTHONIMPORTSOURCE_H

#ifdef JODA_ENABLE_PYTHON

#include <Python.h>
#include <joda/extension/python/PythonModule.h>
#include <joda/parser/IImportSource.h>

namespace joda::docparsing {

/**
 * Represents a directory containing one or multiple JSON files to be imported.
 */
class PythonImportSource : public joda::docparsing::IImportSource {
 public:
  explicit PythonImportSource(
      const std::shared_ptr<extension::python::PythonModule>& pyModule,
      const std::string& paramString, const double sample = 1);
  ~PythonImportSource() override = default;

  virtual PipelineTaskPtr getTask() const override;

  size_t estimatedSize() override;

  const std::string toString() override;

  const std::string toQueryString() override;

 private:
  std::string paramString;
  double sample;
  std::shared_ptr<extension::python::PythonModule> pyModule;
};
}  // namespace joda::docparsing

#endif  // JODA_ENABLE_PYTHON

#endif  // JODA_PYTHONIMPORTSOURCE_H
