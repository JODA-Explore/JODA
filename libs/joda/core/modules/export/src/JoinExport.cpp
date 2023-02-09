//
// Created by Nico on 20/03/2019.
//

#include <utility>

#include "../include/joda/export/JoinExport.h"
#include <joda/join/pipeline/StoreJoin.h>

JoinExport::JoinExport(std::shared_ptr<JoinManager> joinManager)
    : joinManager(std::move(joinManager)) {}

const std::string JoinExport::getTimerName() { return "Join Export"; }


const std::string JoinExport::toString() {
  return "Join at " + joinManager->getName();
}

const std::string JoinExport::toQueryString() {
  return "STORE GROUPED BY " + joinManager->getName();
}

 JoinExport::PipelineTaskPtr JoinExport::getTask() const{
   return std::make_unique<joda::queryexecution::pipeline::tasks::join::StoreJoinTask>(
       joinManager);
 }
