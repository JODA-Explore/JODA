//
// Created by Nico Schäfer on 30/07/18.
//

#include "joda/document/TemporaryOrigin.h"

bool TemporaryOrigin::isReparsable() const { return false; }

std::unique_ptr<IOrigin> TemporaryOrigin::clone() const {
  return std::make_unique<TemporaryOrigin>();
}

std::string TemporaryOrigin::toString() const { return "MEMORY"; }
