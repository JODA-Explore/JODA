#ifndef JODA_MODULETYPE_H
#define JODA_MODULETYPE_H
#include <string>

namespace joda::extension {

enum class ModuleType { AGG, IVAL, INDEX, IMPORT, EXPORT, UNKNOWN };

inline constexpr const char* getModuleTypeString(ModuleType mod) {
  switch (mod) {
    case ModuleType::AGG:
      return "Aggregation";
    case ModuleType::IVAL:
      return "Source";
    case ModuleType::INDEX:
      return "Index";
    case ModuleType::IMPORT:
      return "Import";
    case ModuleType::EXPORT:
      return "Export";
    default:
      return "Unknown";
  }
}

}  // namespace joda::extension

#endif  // JODA_MODULETYPE_H
