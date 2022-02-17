//
// Created by Nico on 09/05/2019.
//

#include "joda/document/IDPositionOrigin.h"

bool IDPositionOrigin::isReparsable() const { return false; }

std::unique_ptr<RJDocument> IDPositionOrigin::reparse(
    RJMemoryPoolAlloc& /*alloc*/) const {
  return nullptr;
}

long IDPositionOrigin::getStart() const { return start; }

void IDPositionOrigin::setStart(long start) { IDPositionOrigin::start = start; }

long IDPositionOrigin::getEnd() const { return end; }

void IDPositionOrigin::setEnd(long end) { IDPositionOrigin::end = end; }

int IDPositionOrigin::getIndex() const { return index; }

void IDPositionOrigin::setIndex(int index) { IDPositionOrigin::index = index; }

IDPositionOrigin::IDPositionOrigin(FILEID id) : id(id) {}

IDPositionOrigin::IDPositionOrigin(FILEID id, long start, long end, int index)
    : id(id), start(start), end(end), index(index) {}

bool IDPositionOrigin::operator<(const IOrigin& x) const {
  if(typeid(*this).before(typeid(x))) return true;
  auto* const other = dynamic_cast<const IDPositionOrigin*>(&x);
  if (other != nullptr) {
   return this->operator<(*other);
  }
  return false;
}

bool IDPositionOrigin::operator<(const IDPositionOrigin& other) const {
  if (id < other.id) {
    return true;
  }
  return start < other.start;
}

bool IDPositionOrigin::operator==(const IDPositionOrigin& other) const {
  return id == other.id && start == other.start && end == other.end &&
           index == other.index;

}