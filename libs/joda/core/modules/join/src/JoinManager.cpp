//
// Created by Nico Schäfer on 08/01/18.
//

#include <glog/logging.h>
#include <joda/join/JoinManager.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <experimental/filesystem>
#include <fstream>
#include <utility>

#include "../include/joda/join/JoinManager.h"

JoinManager::JoinManager(std::string name,
                         std::unique_ptr<joda::query::IValueProvider>&& valProv)
    : name(std::move(name)), valProv(std::move(valProv)) {
  CHECK(this->valProv->isAny() || this->valProv->isAtom());
}

JoinManager::~JoinManager() = default;

const std::string& JoinManager::getName() const { return name; }

void JoinManager::join(JSONContainer& cont) {
  cont.forAll([this](RapidJsonDocument& doc) { this->join(doc); });
}
