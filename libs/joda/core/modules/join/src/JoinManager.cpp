//
// Created by Nico Schäfer on 08/01/18.
//

#include <experimental/filesystem>
#include <fstream>
#include <glog/logging.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <joda/join/JoinManager.h>


#include "../include/joda/join/JoinManager.h"


JoinManager::JoinManager(const std::string &name, std::unique_ptr<joda::query::IValueProvider> &&valProv): name(name), valProv(std::move(valProv)) {
  CHECK(this->valProv->isAny() || this->valProv->isAtom());

}

JoinManager::~JoinManager() {

}


const std::string &JoinManager::getName() const {
  return name;
}

void JoinManager::join(JSONContainer &cont) {
  std::function<void(RapidJsonDocument &)> func = [this](RapidJsonDocument& doc){this->join(doc);};
  cont.forAll(func);
}


