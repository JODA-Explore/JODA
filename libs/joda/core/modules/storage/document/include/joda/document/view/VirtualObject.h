//
// Created by Nico on 27/08/2019.
//

#ifndef JODA_VIRTUALOBJECT_H
#define JODA_VIRTUALOBJECT_H

#include <string>
#include <joda/misc/RJFwd.h>
#include <glog/logging.h>
#include "ViewStructure.h"

class VirtualObject {
 public:
  VirtualObject() = default;
  explicit VirtualObject(ViewStructure *struc);
  VirtualObject(const VirtualObject &o) = delete;
  VirtualObject &operator=(const VirtualObject &o) = delete;
  VirtualObject(VirtualObject &&o) = default;
  VirtualObject &operator=(VirtualObject &&o) = default;

  /*
   * Special Access
   */
  size_t size() const;
  std::vector<std::string> attributes() const;
  RJValue deepCopy(RJMemoryPoolAlloc &alloc) const;

  void addMember(ViewStructure::ID, const RJValue *val);
  void addMember(ViewStructure::ID, const VirtualObject *val);
  void reserve(size_t r);

 private:
  size_t start_ = 0, end_ = 0, size_ = 0;
  ViewStructure *struc = nullptr;

  template<typename F>
  auto iterateMemberRetAll(F &&lambda) const {
    std::vector<typename std::result_of<F(const ViewStructure::ObjectMember &)>::type> ret;
    iterateMember([&ret, &lambda](const ViewStructure::ObjectMember &o) {
      ret.emplace_back(lambda(o));
    });
    return ret;
  }

  template<typename Ret, typename F>
  auto iterateMemberRetOne(F &&lambda, Ret &&init) const {
    Ret ret = init;
    iterateMember([&ret, &lambda](const ViewStructure::ObjectMember &o) {
      ret = lambda(o, ret);
    });
    return ret;
  }

  template<typename F>
  void iterateMember(F &&lambda) const {
    auto it = struc->beginMember();
    it += start_;
    DCHECK(end_ - start_ >= size_);
    for (size_t j = 0; j < size_; ++j) {
      const auto &member = *it;
      lambda(member);
      it++;
    }
  }

 public:
  /*
   * Accepter
   */
  template<typename Handler>
  bool Accept(Handler &handler) const {
    if (RAPIDJSON_UNLIKELY(!handler.StartObject()))
      return false;

    if (!iterateMemberRetOne<bool>([this, &handler](const ViewStructure::ObjectMember &member, bool &prev) {
      if (!prev) return false;
      auto name = struc->getKey(member.key);
      DCHECK(name != nullptr);
      if (RAPIDJSON_UNLIKELY(!handler.Key(name->c_str(),
                                          name->size(),
                                          true)))
        return false;

      if (member.val != nullptr) {
        if (RAPIDJSON_UNLIKELY(!member.val->Accept(handler)))
          return false;
      } else {
        if(member.obj == nullptr){
          if (RAPIDJSON_UNLIKELY(!RJValue().Accept(handler)))
            return false;
        }else{
          if (RAPIDJSON_UNLIKELY(!member.obj->Accept(handler)))
            return false;
        }

      }
      return true;
    }, true))
      return false;

    return handler.EndObject(size_);
  }

};

#endif //JODA_VIRTUALOBJECT_H
