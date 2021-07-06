//
// Created by Nico on 03/06/2019.
//

#ifndef JODA_DELETEPOINTERPROJECTOR_H
#define JODA_DELETEPOINTERPROJECTOR_H

#include "IProjector.h"
namespace joda::query {
class DeletePointerProjector : public IProjector {
 public:
  DeletePointerProjector(const std::string &to);
  ~DeletePointerProjector() override = default;
  std::string getType() override;
  void project(const RapidJsonDocument &json, RJDocument &newDoc,
               bool view = false) override;
  std::string toString() override;
  std::vector<std::string> getMaterializeAttributes() const override;

 protected:
  RJValue getVal(const RapidJsonDocument &json,
                 RJMemoryPoolAlloc &alloc) override;
};
}  // namespace joda::query
#endif  // JODA_DELETEPOINTERPROJECTOR_H
