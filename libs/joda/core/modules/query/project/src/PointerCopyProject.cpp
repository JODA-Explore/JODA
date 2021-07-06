//
// Created by Nico Schäfer on 11/14/17.
//

#include "../include/joda/query/project/PointerCopyProject.h"
#include <rapidjson/fwd.h>
#include <rapidjson/stringbuffer.h>
joda::query::PointerCopyProject::PointerCopyProject(const std::string& to,
                                                    const std::string& from)
    : IProjector(to), from(from.c_str()), from_str(from) {
  if (to.empty() && from.empty()) {
    copyAll = true;
  }
}
RJValue joda::query::PointerCopyProject::getVal(const RapidJsonDocument& json,
                                                RJMemoryPoolAlloc& alloc) {
  if (json.isView()) {
    auto& view = json.getView();
    view->setPrefix(from_str);
    RJDocument doc(&alloc);
    doc.Populate(*view);
    view->setPrefix("");
    RJPointer root("");
    return std::move(*(root.Get(doc)));
  }
  auto val = json.Get(from);
  if (val != nullptr) {
    RJValue newval;
    newval.CopyFrom(*val, alloc, true);
    return newval;
  }

  return RJValue();
}
std::string joda::query::PointerCopyProject::getType() {
  if (copyAll) {
    return allCopy;
  }
  return type;
}
const std::string joda::query::PointerCopyProject::type = "PointerCopy";
const std::string joda::query::PointerCopyProject::allCopy = "AllCopy";

std::string joda::query::PointerCopyProject::toString() {
  if (copyAll) {
    return "*";
  }
  return joda::query::IProjector::toString() + "'" + from_str + "'";
}

std::vector<std::string>
joda::query::PointerCopyProject::getMaterializeAttributes() const {
  return {};
}
