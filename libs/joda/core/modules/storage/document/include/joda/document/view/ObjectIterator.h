//
// Created by Nico on 06/09/2019.
//

#ifndef JODA_OBJECTITERATOR_H
#define JODA_OBJECTITERATOR_H

#include <joda/misc/RJFwd.h>
#include <string>
#include <utility>
#include <vector>
#include "ViewCursor.h"

class ObjectIterator {
 public:
  ObjectIterator(ViewCursor *curs, const RJValue *, ViewStructure *struc);

  void setNext(size_t name) const;

 private:
  typedef std::pair<size_t, const RJValue *> Member;
  typedef std::vector<Member> Members;
  Members members;
  ViewCursor *curs;
};

#endif  // JODA_OBJECTITERATOR_H
