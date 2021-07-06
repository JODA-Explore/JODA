//
// Created by Nico on 20/03/2019.
//

#include <utility>

#include "../include/joda/export/JoinExport.h"

JoinExport::JoinExport(std::shared_ptr<JoinManager> joinManager)
    : joinManager(std::move(joinManager)) {}

const std::string JoinExport::getTimerName() { return "Join Export"; }

void JoinExport::exportContainer(std::unique_ptr<JSONContainer>&& cont) {
  joinManager->join(*cont);
}

const std::string JoinExport::toString() {
  return "Join at " + joinManager->getName();
}

const std::string JoinExport::toQueryString() {
  return "STORE GROUPED BY " + joinManager->getName();
}
