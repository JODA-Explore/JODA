//
// Created by Nico on 27/08/2019.
//

#include "VirtualObject.h"

size_t VirtualObject::size() const { return size_; }

void VirtualObject::addMember(ViewStructure::ID id, const RJValue* val) {
  DCHECK(start_ + size_ < end_);
  DCHECK(val != nullptr);
  struc->addMember(start_ + size_, id, val);
  size_++;
}

void VirtualObject::addMember(ViewStructure::ID id, const VirtualObject* val) {
  DCHECK(start_ + size_ < end_);
  DCHECK(val != nullptr);
  struc->addMember(start_ + size_, id, val);
  size_++;
}

void VirtualObject::reserve(size_t r) {
  auto p = struc->reserve(r);
  start_ = p.first;
  end_ = p.second;
}

std::vector<std::string> VirtualObject::attributes() const {
  return iterateMemberRetAll([this](const ViewStructure::ObjectMember& member) {
    return *this->struc->getKey(member.key);
  });
}

RJValue VirtualObject::deepCopy(RJMemoryPoolAlloc& alloc) const {
  RJValue ret(rapidjson::kObjectType);
  ret.MemberReserve(size_, alloc);

  iterateMember(
      [this, &ret, &alloc](const ViewStructure::ObjectMember& member) {
        RJValue name(*struc->getKey(member.key), alloc);
        RJValue val;
        if (member.val != nullptr) {
          val.CopyFrom(*member.val, alloc, true);
        } else {
          DCHECK(member.obj != nullptr);
          val = member.obj->deepCopy(alloc);
        }
        ret.AddMember(std::move(name), std::move(val), alloc);
      });

  return ret;
}

VirtualObject::VirtualObject(ViewStructure* struc) : struc(struc) {}
