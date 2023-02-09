//
// Created by Nico on 09/05/2019.
//

#include "joda/document/URLOrigin.h"

bool URLOrigin::isReparsable() const { return false; }

std::unique_ptr<RJDocument> URLOrigin::reparse(
    RJMemoryPoolAlloc& /*alloc*/) const {
  return nullptr;
}

std::string URLOrigin::toString() const {
  return std::string("URL: ") + std::to_string(id) + ":" +
         std::to_string(start) + "-" + std::to_string(end);
}

std::unique_ptr<IOrigin> URLOrigin::clone() const {
  return std::make_unique<URLOrigin>(id, start, end, index);
}

std::unique_ptr<IDPositionOrigin> URLOrigin::cloneSpecific() const {
  return std::make_unique<URLOrigin>(id, start, end, index);
}

URLOrigin::URLOrigin(FILEID url) : IDPositionOrigin(url) {}

std::string URLOrigin::getStreamName() const {
  return std::string("URL: ") + g_FileNameRepoInstance.getFile(id);
}

const FILEID& URLOrigin::getUrl() const { return id; }

URLOrigin::URLOrigin(FILEID url, long start, long end, int index)
    : IDPositionOrigin(url, start, end, index) {}
