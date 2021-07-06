//
// Created by Nico on 27/08/2019.
//

#ifndef JODA_VIEWLAYER_H
#define JODA_VIEWLAYER_H

#include <glog/logging.h>
#include <joda/config/config.h>
#include <joda/misc/RJFwd.h>
#include <rapidjson/stringbuffer.h>
#include <algorithm>
#include <mutex>
#include <unordered_map>
#include <variant>
#include "ObjectIterator.h"
#include "ViewCursor.h"
#include "ViewStructure.h"
#include "VirtualObject.h"

class ViewLayer {
 public:
  ViewLayer(const RJValue *doc, const std::vector<std::string> *viewPaths,
            ViewLayer *previousLayer, ViewStructure *struc);

  ViewLayer(const ViewLayer &other) = delete;
  ViewLayer &operator=(const ViewLayer &other) = delete;

  ViewLayer(ViewLayer &&other) = delete;
  ViewLayer &operator=(ViewLayer &&other) = delete;

 private:
  const RJValue *doc;
  std::vector<std::string> const *viewPaths;
  ViewLayer *previousLayer;
  std::string prefix = "";
  std::unordered_map<std::string, VirtualObject> objects;
  typedef typename std::unique_ptr<ViewCursor> Cursor;
  Cursor cursor;

  bool pathOverwritten(const std::string &path) const;
  ViewStructure *struc;

  /*
   * Accepts
   */
 public:
  void setPrefix(const std::string &prefix);

  bool pointerIsOverwritten(const std::string &ptr) const;
  const VirtualObject *getVO(const std::string &ptr) const;

  template <typename Handler>
  bool Accept(Handler &handler) {
    lock();
    getCursor(prefix);
    auto res = Accept(handler, prefix);
    unlock();
    return res;
  }

  template <typename Handler>
  bool operator()(Handler &handler) {
    return Accept(handler);
  }

  RJValue const *Get(const RJPointer &ptr);

  std::variant<const RJValue, std::optional<const RJValue *>,
               const VirtualObject *>
  getPointerIfExists(const std::string &ptr, RJMemoryPoolAlloc &alloc) const {
    auto obj = objects.find(ptr);
    if (obj != objects.end()) {
      return &obj->second;
    }
    auto *v = RJPointer(ptr.c_str()).Get(*doc);
    if (pathOverwritten(ptr)) {
      return v;
    } else {
      // It exists, but is not overwritten => shared
      if (v != nullptr) return std::nullopt;
      // It doesn't exist here, check below
      if (previousLayer != nullptr)
        return previousLayer->getPointerIfExists(ptr, alloc);
      // If no below to check, return nullptr, meaning it is really not here
      return std::optional<const RJValue *>(nullptr);
    }
  };

  template <typename Handler>
  bool Accept(Handler &handler, const std::string &ptr) {
    auto obj = objects.find(ptr);
    if (obj != objects.end()) {
      return obj->second.Accept(handler);
    }

    if (cursor == nullptr) return true;
    if (cursor->isShared(ptr)) {
      return AcceptShared(handler, ptr);
    } else {
      auto *val = cursor->getVal();
      if (val == nullptr) {
        if (previousLayer == nullptr) return handler.Null();
        return previousLayer->Accept(handler, ptr);
      } else {
        return val->Accept(handler);
      }
    }
  }

 private:
  void lock();
  void unlock();
  std::mutex mut;
  RJValue const *Get(const RJPointer &ptr, const std::string &ptrStr);

  void getCursor(const std::string &ptr);

  template <typename Handler>
  bool AcceptShared(Handler &handler, const std::string &curr) {
    auto members = GetMembers(curr);
    auto baseAttPath = curr + "/";
    auto baseAttSize = baseAttPath.size();
    if (RAPIDJSON_UNLIKELY(!handler.StartObject())) return false;

    VirtualObject obj(struc);
    auto vos_enabled =
        config::enable_views_vo && previousLayer != nullptr && struc != nullptr;

    if (vos_enabled) {
      obj.reserve(members.size());
    }
    for (const auto &member : members) {
      if (RAPIDJSON_UNLIKELY(
              !handler.Key(member.name.c_str(), member.name.size(), true)))
        return false;
      if (member.val != nullptr) {
        if (vos_enabled)
          obj.addMember(struc->getOrAdd(member.name), member.val);
        if (RAPIDJSON_UNLIKELY(!member.val->Accept(handler))) return false;
      } else {
        cursor->followAtt(member.name);
        baseAttPath.erase(baseAttSize);
        baseAttPath.append(member.name);
        if (!Accept(handler, baseAttPath)) {
          return false;
        }
        if (vos_enabled) {
          DCHECK(objects.count(baseAttPath) > 0);
          obj.addMember(struc->getOrAdd(member.name), &objects[baseAttPath]);
        }
        cursor->unfollow();
      }
    }

    if (vos_enabled) objects[curr] = std::move(obj);

    return handler.EndObject(members.size());
  }

  std::vector<ViewCursor::ViewObjectMember> GetMembers(
      const std::string &currPath);
};

#endif  // JODA_VIEWLAYER_H
