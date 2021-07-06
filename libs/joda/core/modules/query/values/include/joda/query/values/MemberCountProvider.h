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

  constexpr bool Null() { return true; }

  constexpr bool Bool(bool b) { return true; }

  constexpr bool Int(int i) { return true; }

  constexpr bool Uint(unsigned i) { return true; }

  constexpr bool Int64(int64_t i) { return true; }

  constexpr bool Uint64(uint64_t i) { return true; }

  constexpr bool Double(double d) { return true; }

  constexpr bool RawNumber(const Ch *str, rapidjson::SizeType length,
                           bool copy) {
    return true;
  }

  constexpr bool String(const Ch *str, rapidjson::SizeType length, bool copy) {
    return true;
  }

  bool StartObject() {
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
  }

 private:
  int stack = 0;
  size_t count = 0;
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
    accepter.Accept(json, alloc, handler);
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
