//
// Created by Nico on 16/07/2019.
//
#include <unordered_map>
#include "UnaryPointerAcceptProvider.h"
#include "ValueAccepter.h"
#ifndef JODA_HASHPROVIDER_H
#define JODA_HASHPROVIDER_H
namespace joda::query::providers::hash {
class HashHandler {
 public:
  typedef char Ch;

  bool Null() {
    std::string v = "[JODA]Nullval";
    hash_combine(hash, v);
    return true;
  }

  bool Bool(bool b) {
    hash_combine(hash, b);
    return true;
  }

  bool Int(int i) {
    hash_combine(hash, i);
    return true;
  }

  bool Uint(unsigned i) {
    hash_combine(hash, i);
    return true;
  }

  bool Int64(int64_t i) {
    hash_combine(hash, i);
    return true;
  }

  bool Uint64(uint64_t i) {
    hash_combine(hash, i);
    return true;
  }

  bool Double(double d) {
    hash_combine(hash, d);
    return true;
  }

  bool RawNumber(const Ch *str, rapidjson::SizeType length, bool copy) {
    hash_combine(hash, std::string(str, length));
    return true;
  }

  bool String(const Ch *str, rapidjson::SizeType length, bool copy) {
    hash_combine(hash, std::string(str, length));
    return true;
  }

  bool StartObject() { return true; }

  bool Key(const Ch *str, rapidjson::SizeType length, bool copy) {
    hash_combine(hash, std::string(str, length));
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

  size_t getHash() const { return hash; }

  void reset() {
    stack = 0;
    hash = 0;
  }

 private:
  int stack = 0;
  size_t hash = 0;

  template <class T>
  inline void hash_combine(std::size_t &seed, T const &v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
};

class HashCalculator {
 public:
  static constexpr IValueType retType = IV_Number;
  static constexpr IValueType inType = IV_Any;
  static constexpr bool acceptAll = true;
  static constexpr auto name = "HASH";

  static RJValue accept(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc,
                        const ValueAccepter &accepter) {
    HashHandler handler;
    accepter.Accept(json, alloc, handler);
    return RJValue(handler.getHash());
  }

  static RJValue pointer(const RJValue *ptr) {
    if (ptr == nullptr) return RJValue();
    HashHandler handler;
    ptr->Accept(handler);
    return RJValue(handler.getHash());
  }

  static RJValue virtualObject(const VirtualObject *vo) {
    if (vo == nullptr) return RJValue();
    HashHandler handler;
    vo->Accept(handler);
    return RJValue(handler.getHash());
  }
};
}  // namespace joda::query::providers::hash

namespace joda::query {
template class UnaryPointerAcceptProvider<
    joda::query::providers::hash::HashCalculator>;

typedef UnaryPointerAcceptProvider<joda::query::providers::hash::HashCalculator>
    HashProvider;
}  // namespace joda::query
#endif  // JODA_HASHPROVIDER_H
