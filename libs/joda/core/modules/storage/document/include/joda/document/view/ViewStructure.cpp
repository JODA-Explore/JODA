//
// Created by Nico on 29/08/2019.
//

#include "ViewStructure.h"
#include <joda/misc/RJFwd.h>
#include "VirtualObject.h"

ViewStructure::ID ViewStructure::getOrAdd(const ViewStructure::KEY& key) {
  auto it = key_id.find(key);
  if (it == key_id.end()) {
    auto id = ++curr;
    auto inserted = key_id.insert({key, id});
    id_key[id] = &inserted.first->first;
    return id;
  }
  return it->second;
}

ViewStructure::ID ViewStructure::getOrAdd(ViewStructure::KEY&& key) {
  auto it = key_id.find(key);
  if (it == key_id.end()) {
    auto id = ++curr;
    auto inserted = key_id.insert({std::move(key), id});
    id_key[id] = &inserted.first->first;
    return id;
  }
  auto id = it->second;
  return id;
}

const ViewStructure::KEY* ViewStructure::getKey(
    const ViewStructure::ID& id) const {
  auto it = id_key.find(id);
  if (it == id_key.end()) {
    return nullptr;
  }
  return it->second;
}

std::pair<size_t, size_t> ViewStructure::reserve(size_t size) {
  members.reserve(size * docCount);
  auto start = members.size();
  for (size_t i = 0; i < size; ++i) {
    members.emplace_back();
  }
  auto end = members.size();
  return {start, end};
}

ViewStructure::MemberIterator ViewStructure::beginMember() const {
  return members.begin();
}

void ViewStructure::addMember(size_t i, ViewStructure::ID id,
                              const RJValue* val) {
  DCHECK(i < members.size());
  members[i] = {id, val};
}

void ViewStructure::addMember(size_t i, ViewStructure::ID id,
                              const VirtualObject* val) {
  DCHECK(i < members.size());
  members[i] = {id, val};
}

ViewStructure::ViewStructure(size_t docCount) : docCount(docCount) {}

size_t ViewStructure::estimateSize() const {
  size_t size = 0;
  for (const auto& item : key_id) {
    size += item.first.size() * sizeof(char) + 2 * sizeof(ID);
  }
  size += members.size() * sizeof(ObjectMember);
  return size;
}

ViewStructure::ObjectMember::ObjectMember(ViewStructure::ID key,
                                          const RJValue* val)
    : key(key), val(val), obj(nullptr) {}

ViewStructure::ObjectMember::ObjectMember(ViewStructure::ID key,
                                          const VirtualObject* obj)
    : key(key), val(nullptr), obj(obj) {}