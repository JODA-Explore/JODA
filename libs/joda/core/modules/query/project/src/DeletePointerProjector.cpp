//
// Created by Nico on 03/06/2019.
//

#include "joda/query/project/DeletePointerProjector.h"

joda::query::DeletePointerProjector::DeletePointerProjector(
    const std::string& to)
    : IProjector(to) {}

std::string joda::query::DeletePointerProjector::getType() {
  return "DeletePointer";
}

std::string joda::query::DeletePointerProjector::toString() {
  return IProjector::toString();
}

RJValue joda::query::DeletePointerProjector::getVal(
    const RapidJsonDocument& /*json*/, RJMemoryPoolAlloc& /*alloc*/) {
  return RJValue();
}

void joda::query::DeletePointerProjector::project(const RapidJsonDocument& json,
                                                  RJDocument& newDoc,
                                                  bool view) {
  if (view && ptr.GetTokenCount() > 0) {  // Is View and not root node?
    RJPointer parent(ptr.GetTokens(), ptr.GetTokenCount() - 1);  // Get Parent
    auto vValue = parent.Get(newDoc);
    if (vValue != nullptr) {  // View does not contain the parent
      auto pValue = json.Get(parent);
      if (pValue != nullptr) {  // If in original document parent is an object,
                                // create empty object in view
        if (pValue->IsObject()) {
          parent.Set(newDoc, RJValue(rapidjson::kObjectType));
        }
      }
    }
  }
  ptr.Erase(newDoc);
}

std::vector<std::string>
joda::query::DeletePointerProjector::getMaterializeAttributes() const {
  return std::vector<std::string>();
}
