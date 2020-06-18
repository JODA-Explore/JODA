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
  std::string toString() override;
 protected:
  RJValue getVal(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) override;
  RJPointer from;
};
}

#endif //JODA_POINTERCOPYPROJECT_H
