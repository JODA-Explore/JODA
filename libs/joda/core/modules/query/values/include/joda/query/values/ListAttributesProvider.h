//
// Created by Nico Schäfer on 12/7/17.
//

#ifndef JODA_LISTATTRIBUTESPROVIDER_H
#define JODA_LISTATTRIBUTESPROVIDER_H

#include "IValueProvider.h"
#include "ParameterPack.h"
#include "TemplateProvider.h"
namespace joda::query::providers::listattributes {

class ListAttributesHandler {
 public:
  typedef char Ch;

  constexpr bool Null() {
    if (!firstObject) return false;
    return true;
  }

  constexpr bool Bool(bool b) {
    if (!firstObject) return false;
    return true;
  }

  constexpr bool Int(int i) {
    if (!firstObject) return false;
    return true;
  }

  constexpr bool Uint(unsigned i) {
    if (!firstObject) return false;
    return true;
  }

  constexpr bool Int64(int64_t i) {
    if (!firstObject) return false;
    return true;
  }

  constexpr bool Uint64(uint64_t i) {
    if (!firstObject) return false;
    return true;
  }

  constexpr bool Double(double d) {
    if (!firstObject) return false;
    return true;
  }

  constexpr bool RawNumber(const Ch *str, rapidjson::SizeType length,
                           bool copy) {
    if (!firstObject) return false;
    return true;
  }

  constexpr bool String(const Ch *str, rapidjson::SizeType length, bool copy) {
    if (!firstObject) return false;
    return true;
  }

  bool StartObject() {
    if (stack == 0) firstObject = true;
    stack++;
    return true;
  }

  bool Key(const Ch *str, rapidjson::SizeType length, bool copy) {
    if (!firstObject) return false;
    if (stack == 1) attributes.emplace_back(str, length);
    return true;
  }

  bool EndObject(rapidjson::SizeType memberCount) {
    stack--;
    return true;
  }

  constexpr bool StartArray() {
    if (!firstObject) return false;
    stack++;
    return true;
  }

  constexpr bool EndArray(rapidjson::SizeType elementCount) {
    stack--;
    return true;
  }

  std::vector<std::string> getAttributes() const { return attributes; }

  void reset() {
    attributes.clear();
    stack = 0;
  }

 private:
  std::vector<std::string> attributes;
  int stack = 0;
  bool firstObject = false;
};

class ListAttributesCalculator {
 public:
  // Parameters
  typedef values::ObjectParameter<0> P0;
  typedef values::NoParameter<1> P1;
  typedef values::NoParameter<2> P2;
  typedef values::NoParameter<3> P3;
  typedef values::NoParameter<4> P4;

  // State
  typedef bool State;

  // Return value of the function
  static constexpr IValueType retType = IV_Array;

  static constexpr auto name = "LISTATTRIBUTES";
  /**
   * Checks additional parameter conditions
   * @param parameters the list of passed parameter values
   * @param name the name of the function
   **/
  static void checkParameters(
      State &state,
      const std::vector<std::unique_ptr<IValueProvider>> &parameters){

  };

  static RJValue calculate(
      const State &state,
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    auto accepter = P0::extractValue(parameters, json, alloc);
    RJValue ret(rapidjson::kArrayType);

    auto ptr = accepter.getPointer(json, alloc);
    if (ptr != nullptr) {
      if (!ptr->IsObject()) return RJValue();
      ret.Reserve(ptr->MemberCount(), alloc);
      for (typename RJDocument::ConstMemberIterator m = ptr->MemberBegin();
           m != ptr->MemberEnd(); ++m) {
        ret.PushBack(RJValue(m->name, alloc), alloc);
      }
      return ret;
    }

    auto vo = accepter.getObjVO(json, alloc);
    if (vo != nullptr) {
      ret.Reserve(vo->size(), alloc);
      for (const auto &mem : vo->attributes()) {
        ret.PushBack(RJValue(mem, alloc), alloc);
      }
      return ret;
    }

    ListAttributesHandler handler;
    auto res = accepter.Accept(json, alloc, handler);
    if (!res) return RJValue();

    auto atts = handler.getAttributes();
    ret.Reserve(atts.size(), alloc);
    for (auto &&att : atts) {
      ret.PushBack(RJValue(att, alloc), alloc);
    }
    return ret;
  };
};
}  // namespace joda::query::providers::listattributes

namespace joda::query {
template class TemplateProvider<
    joda::query::providers::listattributes::ListAttributesCalculator>;

typedef TemplateProvider<
    joda::query::providers::listattributes::ListAttributesCalculator>
    ListAttributesProvider;
}  // namespace joda::query

#endif  // JODA_LISTATTRIBUTESPROVIDER_H
