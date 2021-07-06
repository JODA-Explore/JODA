//
// Created by Nico Schäfer on 11/14/17.
//

#ifndef JODA_POINTERCOPYPROJECT_H
#define JODA_POINTERCOPYPROJECT_H

#include "IProjector.h"

namespace joda::query {
/**
 * Transformation copying a value from the source to destination document
 */
class PointerCopyProject : public joda::query::IProjector {
 public:
  PointerCopyProject(const std::string &to, const std::string &from);
  std::string getType() override;
  static const std::string type;
  static const std::string allCopy;
  std::string toString() override;
  std::vector<std::string> getMaterializeAttributes() const override;

 protected:
  RJValue getVal(const RapidJsonDocument &json,
                 RJMemoryPoolAlloc &alloc) override;
  RJPointer from;
  std::string from_str;
  bool copyAll = false;
};
}  // namespace joda::query

#endif  // JODA_POINTERCOPYPROJECT_H
