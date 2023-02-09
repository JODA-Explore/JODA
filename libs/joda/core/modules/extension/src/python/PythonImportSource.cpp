#include <joda/extension/python/PythonImportSource.h>
#include <joda/extension/python/PythonImportTask.h>

#ifdef JODA_ENABLE_PYTHON

namespace joda::docparsing {

PythonImportSource::PythonImportSource(
    const std::shared_ptr<extension::python::PythonModule>& pyModule,
    const std::string& paramString, const double sample)
    : paramString(paramString), sample(sample), pyModule(pyModule) {}

size_t PythonImportSource::estimatedSize() { return 0; }

const std::string joda::docparsing::PythonImportSource::toString() {
  return pyModule->getName();
}

const std::string joda::docparsing::PythonImportSource::toQueryString() {
  std::string sampleString;
  if (sample < 1) {
    sampleString = " SAMPLE " + std::to_string(sample);
  }

  return "FROM " + pyModule->getName() + sampleString;
}

PythonImportSource::PipelineTaskPtr PythonImportSource::getTask() const {
  return std::make_unique<
      queryexecution::pipeline::tasks::load::PythonImportTask>(
      pyModule, paramString, sample);
}

}  // namespace joda::docparsing

#endif  // JODA_ENABLE_PYTHON