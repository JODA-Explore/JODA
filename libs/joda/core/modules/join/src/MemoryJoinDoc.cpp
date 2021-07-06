//
// Created by Nico Schäfer on 02/04/18.
//

#include "../include/joda/join/MemoryJoinDoc.h"

MemoryJoinDoc::MemoryJoinDoc() : doc(&alloc) {
  doc.SetObject();
  RJValue name;
  name.SetString("join");
  RJValue join;
  join.SetArray();
  doc.AddMember(name, join, doc.GetAllocator());
}

MemoryJoinDoc::MemoryJoinDoc(long i) : MemoryJoinDoc() {
  RJValue name;
  name.SetString("value");
  doc.AddMember(name, i, doc.GetAllocator());
}

MemoryJoinDoc::MemoryJoinDoc(const std::string& str) : MemoryJoinDoc() {
  RJValue name;
  name.SetString("value");
  RJValue val;
  val.SetString(str.c_str(), doc.GetAllocator());
  doc.AddMember(name, val, doc.GetAllocator());
}

MemoryJoinDoc::MemoryJoinDoc(bool b) : MemoryJoinDoc() {
  RJValue name;
  name.SetString("value");
  doc.AddMember(name, b, doc.GetAllocator());
}

void MemoryJoinDoc::join(RJValue&& val) {
  std::lock_guard<std::mutex> guard(mut);
  doc["join"].PushBack(std::move(val), doc.GetAllocator());
}

void MemoryJoinDoc::deepCopyInto(RJDocument& val) {
  std::lock_guard<std::mutex> guard(mut);
  val.CopyFrom(doc, val.GetAllocator());
}

size_t MemoryJoinDoc::getMemSize() { return alloc.Size(); }

void MemoryJoinDoc::join(const RJValue& val) {
  std::lock_guard<std::mutex> guard(mut);
  RJDocument tmpdoc;
  tmpdoc.CopyFrom(val, doc.GetAllocator());
  doc["join"].PushBack(std::move(tmpdoc), doc.GetAllocator());
}
