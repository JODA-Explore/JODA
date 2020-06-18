//
// Created by Nico Schäfer on 11/14/17.
//

#include "../include/joda/query/project/PointerCopyProject.h"
#include <rapidjson/fwd.h>
#include <rapidjson/stringbuffer.h>
joda::query::PointerCopyProject::PointerCopyProject(const std::string &to, const std::string &from)
    : IProjector(to), from(from.c_str()) {

}
RJValue joda::query::PointerCopyProject::getVal(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
  auto val = from.Get(*json.getJson());
  if (val != nullptr) {
    RJValue newval;
    newval.CopyFrom(*val, alloc);
    return newval;
  }
  return RJValue();
}
std::string joda::query::PointerCopyProject::getType() {
  return type;
}
const std::string joda::query::PointerCopyProject::type = "PointerCopy";
std::string joda::query::PointerCopyProject::toString() {
  rapidjson::StringBuffer sb;
  from.Stringify(sb);
  return joda::query::IProjector::toString() + "'" + sb.GetString() + "'";
}
