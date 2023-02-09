
#include "UnaryPointerAcceptProvider.h"
#include "ValueAccepter.h"
#ifndef JODA_TRUTHRY_FALSY_PROVIDER_H
#define JODA_TRUTHRY_FALSY_PROVIDER_H
namespace joda::query::providers::truthyfalsy {
class FalsyHandler {
 public:
  typedef char Ch;

  constexpr bool Null() {
    visited = true;
    falsy = true;
    return true;
  }

  constexpr bool Bool(bool b) {
    visited = true;
    falsy = !b;
    return true;
  }

  constexpr bool Int(int i) {
    visited = true;
    falsy = i == 0;
    return true;
  }

  constexpr bool Uint(unsigned i) {
    visited = true;
    falsy = i == 0;
    return true;
  }

  constexpr bool Int64(int64_t i) {
    visited = true;
    falsy = i == 0;
    return true;
  }

  constexpr bool Uint64(uint64_t i) {
    visited = true;
    falsy = i == 0;
    return true;
  }

  constexpr bool Double(double d) {
    visited = true;
    falsy = (std::isnan(d) || d == 0.0);
    return true;
  }

  constexpr bool RawNumber(const Ch *str, rapidjson::SizeType length,
                           bool copy) {
    return true;
  }

  constexpr bool String(const Ch *str, rapidjson::SizeType length, bool copy) {
    visited = true;
    falsy = length == 0;
    return true;
  }

  bool StartObject() {
    visited = true;
    falsy = false;
    return false;
  }

  bool Key(const Ch *str, rapidjson::SizeType length, bool copy) {
    visited = true;
    falsy = false;
    return false;
  }

  bool EndObject(rapidjson::SizeType memberCount) {
    visited = true;
    falsy = false;
    return false;
  }

  constexpr bool StartArray() {
    visited = true;
    falsy = false;
    return false;
  }

  constexpr bool EndArray(rapidjson::SizeType elementCount) {
    visited = true;
    falsy = false;
    return false;
  }

  std::optional<bool> isFalsy() const {
    if (visited) {
      return std::make_optional(falsy);
    } else {
      return std::nullopt;
    }
  }

  void reset() {
    visited = false;
    falsy = false;
  }

 private:
  bool visited = false;
  bool falsy = false;
};

class TruthyCalculator {
 public:
  static constexpr IValueType retType = IV_Bool;
  static constexpr IValueType inType = IV_Any;
  static constexpr bool acceptAll = true;
  static constexpr auto name = "TRUTHY";

  static RJValue accept(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc,
                        const ValueAccepter &accepter) {
    FalsyHandler handler;
    auto res = accepter.Accept(json, alloc, handler);
    if (!res) return RJValue();
    auto falsy = handler.isFalsy();
    if(falsy.has_value()) {
      return RJValue(!falsy.value());
    } else {
      return RJValue();
    }
  }

  static RJValue pointer(const RJValue *ptr) {
    if (ptr == nullptr) return RJValue();
    FalsyHandler handler;
    auto res = ptr->Accept(handler);
    if (!res) return RJValue();
    auto falsy = handler.isFalsy();
    if(falsy.has_value()) {
      return RJValue(!falsy.value());
    } else {
      return RJValue();
    }
  }

  static RJValue virtualObject(const VirtualObject *vo) {
    if (vo == nullptr) return RJValue();
    return RJValue(true);
  }
};

class FalsyCalculator {
 public:
  static constexpr IValueType retType = IV_Bool;
  static constexpr IValueType inType = IV_Any;
  static constexpr bool acceptAll = true;
  static constexpr auto name = "FALSY";

  static RJValue accept(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc,
                        const ValueAccepter &accepter) {
    FalsyHandler handler;
    auto res = accepter.Accept(json, alloc, handler);
    if (!res) return RJValue(true);
    auto falsy = handler.isFalsy();
    if(falsy.has_value()) {
      return RJValue(falsy.value());
    } else {
      return RJValue();
    }
  }

  static RJValue pointer(const RJValue *ptr) {
    if (ptr == nullptr) return RJValue(true);
    FalsyHandler handler;
    auto res = ptr->Accept(handler);
    if (!res) return RJValue();
    auto falsy = handler.isFalsy();
    if(falsy.has_value()) {
      return RJValue(falsy.value());
    } else {
      return RJValue();
    }
  }

  static RJValue virtualObject(const VirtualObject *vo) {
    if (vo == nullptr) return RJValue(true);
    return RJValue(false);
  }
};
}  // namespace joda::query::providers::truthyfalsy

namespace joda::query {
template class UnaryPointerAcceptProvider<
    joda::query::providers::truthyfalsy::FalsyCalculator>;

typedef UnaryPointerAcceptProvider<
    joda::query::providers::truthyfalsy::FalsyCalculator>
    FalsyProvider;

template class UnaryPointerAcceptProvider<
    joda::query::providers::truthyfalsy::TruthyCalculator>;

typedef UnaryPointerAcceptProvider<
    joda::query::providers::truthyfalsy::TruthyCalculator>
    TruthyProvider;
}  // namespace joda::query
#endif  // JODA_TRUTHRY_FALSY_PROVIDER_H
