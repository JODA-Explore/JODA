//
// Created by Nico on 09/05/2019.
//

#include "joda/document/InStreamOrigin.h"

bool InStreamOrigin::isReparsable() const { return false; }

std::unique_ptr<RJDocument> InStreamOrigin::reparse(
    RJMemoryPoolAlloc& /*alloc*/) const {
  return nullptr;
}

std::string InStreamOrigin::toString() const {
  return std::string("STREAM: ") + std::to_string(id) + ":" +
         std::to_string(start) + "-" + std::to_string(end);
}

std::unique_ptr<IOrigin> InStreamOrigin::clone() const {
  return std::make_unique<InStreamOrigin>(start, end, index);
}

std::unique_ptr<IDPositionOrigin> InStreamOrigin::cloneSpecific() const {
  return std::make_unique<InStreamOrigin>(start, end, index);
}

InStreamOrigin::InStreamOrigin() : IDPositionOrigin(g_FileNameRepoInstance.addFile("STDIN")) {}

std::string InStreamOrigin::getStreamName() const {
  return std::string("INSTREAM") ;
}


InStreamOrigin::InStreamOrigin(long start, long end, int index)
    : IDPositionOrigin(g_FileNameRepoInstance.addFile("STDIN"),start, end, index) {}
