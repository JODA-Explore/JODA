//
// Created by Nico Schäfer on 11/13/17.
//

#ifndef JODA_POINTERPROVIDER_H
#define JODA_POINTERPROVIDER_H

#include <rapidjson/fwd.h>
#include <rapidjson/pointer.h>
#include <variant>
#include "IValueProvider.h"


namespace joda::query {
/**
 * '<JSON Pointer>', a pointer to a specific value in a document
 */
class PointerProvider : public joda::query::IValueProvider {
 public:
  PointerProvider(const std::string &str);

  bool comparable() const override;
  bool equalizable() const override;
  std::unique_ptr<IValueProvider> duplicate() const override;
  std::string toString() const override;
  std::string getAttributeString() const;
  bool isConst() const override;
  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;
  void getAttributes(std::vector<std::string> &vec) const override;
  std::vector<std::string> getAttributes() const override;
  bool isString() const override;
  bool isNumber() const override;
  bool isBool() const override;
  bool isObject() const override;
  bool isArray() const override;
  bool isAny() const override;
  std::string getName() const override;
  IValueType getReturnType() const override;

  /**
   *  Returns either a atomic RJValue, an optional pointer or a VirtualObject,
   *  depending on the existing indices and values.
   *  @param json the document to check
   *  @param alloc the allocator to use for allocation operations
   */
  std::variant<const RJValue,
               std::optional<const RJValue *>,
               const VirtualObject *> getPointerIfExists(const RapidJsonDocument &json,
                                                         RJMemoryPoolAlloc &alloc) const;
 protected:
  RJPointer pointer;
  std::string ptrStr;

 public:
  template<class Handler>
  bool Accept(const RapidJsonDocument &json,
              RJMemoryPoolAlloc &alloc, Handler &h) const {
    if (json.isView()) {
      auto &v = json.getView();
      DCHECK(v != nullptr);
      if (ptrStr.empty()) {
        return v->Accept(h);
      } else {

        v->setPrefix(ptrStr);
        auto res = v->Accept(h);
        v->setPrefix("");
        return res;
      }
    } else {
      auto ptr = json.Get(pointer);
      if (ptr != nullptr) {
        return ptr->Accept(h);
      } else h.Null();
      return true;
    }
  }

  bool objIsPointerEvaluatable(const RapidJsonDocument &json) const;
  const VirtualObject *getVO(const RapidJsonDocument &json) const;

};
}

#endif  // JODA_POINTERPROVIDER_H
