//
// Created by Nico on 27/08/2019.
//

#include "ViewLayer.h"
#include <algorithm>
#include <unordered_map>

bool ViewLayer::pathOverwritten(const std::string& path) const {
  if (viewPaths == nullptr) {
    return true;
  }
  return std::find_if(viewPaths->begin(), viewPaths->end(),
                      [&path](const std::string& p) {
                        return p.size() <= path.size() && path.find(p) == 0;
                      }) != viewPaths->end();
}

ViewLayer::ViewLayer(const RJValue* doc,
                     const std::vector<std::string>* viewPaths,
                     ViewLayer* previousLayer, ViewStructure* struc)
    : doc(doc),
      viewPaths(viewPaths),
      previousLayer(previousLayer),
      struc(struc) {}

void ViewLayer::getCursor(const std::string& ptr) {
  if (cursor != nullptr) {
    cursor->reset();
  } else {
    if (previousLayer != nullptr) {
      previousLayer->getCursor("");
    }
    if (doc == nullptr) {
      return;
    }

    if (previousLayer == nullptr) {
      cursor = std::make_unique<ViewCursor>(doc, nullptr, viewPaths);
    } else {
      cursor = std::make_unique<ViewCursor>(doc, previousLayer->cursor.get(),
                                            viewPaths);
    }
  }
  if (!ptr.empty()) {
    RJPointer rjPointer(ptr);
    cursor->follow(rjPointer);
  }
}

RJValue const* ViewLayer::Get(const RJPointer& ptr) {
  rapidjson::StringBuffer ss;
  ptr.Stringify(ss);
  auto str = ss.GetString();
  return Get(ptr, str);
}

RJValue const* ViewLayer::Get(const RJPointer& ptr, const std::string& ptrStr) {
  if (doc != nullptr && pathOverwritten(ptrStr)) {
    return ptr.Get(*doc);
  }
  if (previousLayer == nullptr) {
    return nullptr;
  }
  return previousLayer->Get(ptr, ptrStr);
}

void ViewLayer::setPrefix(const std::string& prefix) {
  ViewLayer::prefix = prefix;
}

bool ViewLayer::pointerIsOverwritten(const std::string& ptr) const {
  if (viewPaths == nullptr) {
    return true;
  }
  return std::find(viewPaths->begin(), viewPaths->end(), ptr) !=
         viewPaths->end();
}

void ViewLayer::updateDoc(const RJValue *doc) {
  this->doc = doc;
}

const VirtualObject* ViewLayer::getVO(const std::string& ptr) const {
  auto obj = objects.find(ptr);
  if (obj != objects.end()) {
    return &obj->second;
  }
  return nullptr;
}

void ViewLayer::lock() {
  if (previousLayer != nullptr) {
    previousLayer->lock();
  }
  mut.lock();
}

void ViewLayer::unlock() {
  if (previousLayer != nullptr) {
    previousLayer->unlock();
  }
  mut.unlock();
}

std::vector<ViewCursor::ViewObjectMember> ViewLayer::GetMembers(
    const std::string& currPath) {
  auto* val = cursor->getVal();
  if (cursor->isBase(currPath)) {
    std::vector<ViewCursor::ViewObjectMember> members{};
    if (val != nullptr && val->IsObject()) {
      for (typename RJDocument::ConstMemberIterator m = val->MemberBegin();
           m != val->MemberEnd(); ++m) {
        members.emplace_back(
            std::string(m->name.GetString(), m->name.GetStringLength()),
            &m->value);
      }
    }
    return members;
  }
  DCHECK(previousLayer != nullptr);
  auto members = previousLayer->GetMembers(currPath);
  if (val != nullptr &&
      val->IsObject()) {  // Val is either object, or null if no changes are
                          // written to the delta tree
    members.erase(std::remove_if(
                      members.begin(), members.end(),
                      [&val, &currPath, this](auto& member) {
                        auto tmpPath = currPath + "/" + member.name;
                        return std::find(viewPaths->begin(), viewPaths->end(),
                                         tmpPath) != viewPaths->end() &&
                               val->FindMember(member.name) == val->MemberEnd();
                      }),
                  members.end());
    for (typename RJDocument::ConstMemberIterator m = val->MemberBegin();
         m != val->MemberEnd(); ++m) {
      auto name = std::string(m->name.GetString(), m->name.GetStringLength());
      auto iterator =
          std::find_if(members.begin(), members.end(),
                       [&name](auto& member) { return member.name == name; });
      if (iterator == members.end()) {
        members.emplace_back(std::move(name), &m->value);
      } else {
        auto tmpPath = currPath + "/" + name;
        if (std::find(viewPaths->begin(), viewPaths->end(), tmpPath) !=
            viewPaths->end()) {
          iterator->val = &m->value;
        } else {
          iterator->val = nullptr;
        }
      }
    }
  }

  return members;
}
