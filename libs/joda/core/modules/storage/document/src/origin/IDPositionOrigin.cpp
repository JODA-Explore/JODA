//
// Created by Nico on 09/05/2019.
//

#include "joda/document/IDPositionOrigin.h"

bool IDPositionOrigin::isReparsable() const {
  return false;
}

std::shared_ptr<RJDocument> IDPositionOrigin::reparse(RJMemoryPoolAlloc &alloc) const {
  return nullptr;
}

long IDPositionOrigin::getStart() const {
  return start;
}

void IDPositionOrigin::setStart(long start) {
  IDPositionOrigin::start = start;
}

long IDPositionOrigin::getEnd() const {
  return end;
}

void IDPositionOrigin::setEnd(long end) {
  IDPositionOrigin::end = end;
}

int IDPositionOrigin::getIndex() const {
  return index;
}

void IDPositionOrigin::setIndex(int index) {
  IDPositionOrigin::index = index;
}

IDPositionOrigin::IDPositionOrigin(FILEID id) : id(id) {}

IDPositionOrigin::IDPositionOrigin(FILEID id, long start, long end, int index)
    : id(id), start(start), end(end), index(index) {}

bool IDPositionOrigin::operator<(const IOrigin &x) const {
  if (typeid(*this) == typeid(x)) {
    const auto
        &other = dynamic_cast<const IDPositionOrigin &>(x); //Static cast, as type is already checked to be the same
    if (id < other.id) return true;
    return start < other.start;
  }
  return IOrigin::operator<(x);
}

bool IDPositionOrigin::operator==(const IOrigin &x) const {
  if (typeid(*this) == typeid(x)) {
    const auto
        &other = dynamic_cast<const IDPositionOrigin &>(x); //Static cast, as type is already checked to be the same
    return id == other.id && start == other.start && end == other.end && index == other.index;
  }
  return IOrigin::operator==(x);
}