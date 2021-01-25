//
// Created by Nico on 06/09/2019.
//

#include <algorithm>
#include "ObjectIterator.h"

ObjectIterator::ObjectIterator(ViewCursor *curs, const RJValue *val, ViewStructure *struc) : curs(curs) {
  if (val == nullptr) return;
  if (!val->IsObject()) return;

  members.reserve(val->MemberCount());
  for (typename RJDocument::ConstMemberIterator m = val->MemberBegin(); m != val->MemberEnd(); ++m) {
    members.emplace_back(struc->getOrAdd(m->name.GetString()), &m->value);
  }
}

void ObjectIterator::setNext(size_t name) const {
  auto it = std::find_if(members.begin(), members.end(), [&name](const Member &m) {
    return m.first == name;
  });
  if (it != members.end()) {
    curs->setNextDestination(it->second);
  } else {
    curs->setNextDestination(nullptr);
  }
}
