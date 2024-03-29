//
// Created by Nico on 16/07/2019.
//
#include "UnaryPointerAcceptProvider.h"
#include "ValueAccepter.h"
#ifndef JODA_MEMBERCOUNTPROVIDER_H
#define JODA_MEMBERCOUNTPROVIDER_H
namespace joda::query::providers::membercount {
class MemberCountHandler {
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
    if (stack == 1) count++;
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

  size_t getCount() const { return count; }

  void reset() {
    stack = 0;
    count = 0;
    firstObject = false;
  }

 private:
  int stack = 0;
  size_t count = 0;
  bool firstObject = false;
};

class MemberCountCalculator {
 public:
  static constexpr IValueType retType = IV_Number;
  static constexpr IValueType inType = IV_Object;
  static constexpr bool acceptAll = false;
  static constexpr auto name = "MEMCOUNT";

  static RJValue accept(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc,
                        const ValueAccepter &accepter) {
    MemberCountHandler handler;
    auto res = accepter.Accept(json, alloc, handler);
    if(!res) return RJValue();
    return RJValue(handler.getCount());
  }

  static RJValue pointer(const RJValue *ptr) {
    if (ptr == nullptr || !ptr->IsObject()) return RJValue();
    return RJValue(ptr->MemberCount());
  }

  static RJValue virtualObject(const VirtualObject *vo) {
    if (vo == nullptr) return RJValue();
    return RJValue(vo->size());
  }
};
}  // namespace joda::query::providers::membercount

namespace joda::query {
template class UnaryPointerAcceptProvider<
    joda::query::providers::membercount::MemberCountCalculator>;

typedef UnaryPointerAcceptProvider<
    joda::query::providers::membercount::MemberCountCalculator>
    MemberCountProvider;
}  // namespace joda::query
#endif  // JODA_MEMBERCOUNTPROVIDER_H
