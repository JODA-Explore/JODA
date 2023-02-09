
#ifndef JODA_ITERATORPROVIDER_H
#define JODA_ITERATORPROVIDER_H

#include "IValueProvider.h"
#include "ParameterPack.h"
#include "TemplateProvider.h"
namespace joda::query::providers::iterator {

class ExtractIteratorHandler {
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
    if (stack == 0) firstObject = true;
    stack++;
    return true;
  }

  constexpr bool EndArray(rapidjson::SizeType elementCount) {
    if (!firstObject) return false;
    if (stack == 1) arr_size = elementCount;
    stack--;
    return true;
  }

  std::vector<std::string> getAttributes() const {
    if (!attributes.empty()) return attributes;
    std::vector<std::string> array_members;
    for (unsigned int i = 0; i < arr_size; i++) {
      array_members.emplace_back(std::to_string(i));
    }
    return array_members;
  }

  void reset() {
    attributes.clear();
    firstObject = false;
    arr_size = 0;
    stack = 0;
  }

 private:
  std::vector<std::string> attributes;
  size_t arr_size = 0;
  int stack = 0;
  bool firstObject = false;
};

class AnyCalculator {
 public:
  // Parameters
  typedef values::IteratableParameter<0> P0;
  typedef values::BoolParameter<1> P1;
  typedef values::NoParameter<2> P2;
  typedef values::NoParameter<3> P3;
  typedef values::NoParameter<4> P4;

  // State => Pointer Prefix
  typedef std::string State;

  // Return value of the function
  static constexpr IValueType retType = IV_Bool;

  static constexpr auto name = "ANY";
  /**
   * Checks additional parameter conditions
   * @param parameters the list of passed parameter values
   * @param name the name of the function
   **/
  static void checkParameters(
      State &state,
      const std::vector<std::unique_ptr<IValueProvider>> &parameters) {
    auto ptr = dynamic_cast<PointerProvider *>(parameters[0].get());
    if (ptr == nullptr) {
      throw std::runtime_error("Parameter 0 of ANY must be a pointer");
    }
    state = ptr->getAttributes()[0];
  };

  static RJValue calculate(
      const State &state,
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    auto accepter = P0::extractValue(parameters, json, alloc);
    ExtractIteratorHandler handler;
    accepter.Accept(json, alloc, handler);

    auto attributes = handler.getAttributes();
    bool ret = false;
    for (auto &attr : attributes) {
      auto prefix = state + "/" + attr;
      auto param = parameters[1]->duplicate();
      param->prependAttributes(prefix);
      ret = ret || param->getNonTruthyBool(json, alloc);  // OR
      if (ret) return RJValue(true);                      // Lazy evaluation
    }

    return RJValue(ret);
  }
};

class AllCalculator {
 public:
  // Parameters
  typedef values::IteratableParameter<0> P0;
  typedef values::BoolParameter<1> P1;
  typedef values::NoParameter<2> P2;
  typedef values::NoParameter<3> P3;
  typedef values::NoParameter<4> P4;

  // State => Pointer Prefix
  typedef std::string State;

  // Return value of the function
  static constexpr IValueType retType = IV_Bool;

  static constexpr auto name = "ALL";
  /**
   * Checks additional parameter conditions
   * @param parameters the list of passed parameter values
   * @param name the name of the function
   **/
  static void checkParameters(
      State &state,
      const std::vector<std::unique_ptr<IValueProvider>> &parameters) {
    auto ptr = dynamic_cast<PointerProvider *>(parameters[0].get());
    if (ptr == nullptr) {
      throw std::runtime_error("Parameter 0 of ALL must be a pointer");
    }
    state = ptr->getAttributes()[0];
  };

  static RJValue calculate(
      const State &state,
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    auto accepter = P0::extractValue(parameters, json, alloc);
    ExtractIteratorHandler handler;
    accepter.Accept(json, alloc, handler);

    auto attributes = handler.getAttributes();
    bool ret = true;
    for (auto &attr : attributes) {
      auto prefix = state + "/" + attr;
      auto param = parameters[1]->duplicate();
      param->prependAttributes(prefix);
      ret = ret && param->getNonTruthyBool(json, alloc);  // AND
      if (!ret) return RJValue(false);                    // Lazy Evaluation
    }

    return RJValue(ret);
  }
};

class MapCalculator {
 public:
  // Parameters
  typedef values::ArrayParameter<0> P0;
  typedef values::AnyParameter<1> P1;
  typedef values::NoParameter<2> P2;
  typedef values::NoParameter<3> P3;
  typedef values::NoParameter<4> P4;

  // State => Pointer Prefix
  typedef std::string State;

  // Return value of the function
  static constexpr IValueType retType = IV_Array;

  static constexpr auto name = "MAP";
  /**
   * Checks additional parameter conditions
   * @param parameters the list of passed parameter values
   * @param name the name of the function
   **/
  static void checkParameters(
      State &state,
      const std::vector<std::unique_ptr<IValueProvider>> &parameters) {
    auto ptr = dynamic_cast<PointerProvider *>(parameters[0].get());
    if (ptr == nullptr) {
      throw std::runtime_error("Parameter 0 of MAP must be a pointer");
    }
    state = ptr->getAttributes()[0];
  };

  static RJValue calculate(
      const State &state,
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    auto accepter = P0::extractValue(parameters, json, alloc);
    ExtractIteratorHandler handler;
    accepter.Accept(json, alloc, handler);

    auto attributes = handler.getAttributes();
    RJValue arr(rapidjson::kArrayType);
    for (auto &attr : attributes) {
      auto prefix = state + "/" + attr;
      auto param = parameters[1]->duplicate();
      param->prependAttributes(prefix);
      if (param->isAtom()) {
        auto entry = param->getAtomValue(json, alloc);
        arr.PushBack(entry, alloc);
      } else {
        const auto *entry = param->getValue(json, alloc);
        if (entry == nullptr) continue;
        RJDocument copy(&alloc);
        copy.CopyFrom(*entry, alloc);
        arr.PushBack(copy, alloc);
      }
    }
    return arr;
  }
};

class FilterCalculator {
 public:
  // Parameters
  typedef values::ArrayParameter<0> P0;
  typedef values::BoolParameter<1> P1;
  typedef values::NoParameter<2> P2;
  typedef values::NoParameter<3> P3;
  typedef values::NoParameter<4> P4;

  // State => Pointer Prefix
  typedef std::string State;

  // Return value of the function
  static constexpr IValueType retType = IV_Array;

  static constexpr auto name = "FILTER";
  /**
   * Checks additional parameter conditions
   * @param parameters the list of passed parameter values
   * @param name the name of the function
   **/
  static void checkParameters(
      State &state,
      const std::vector<std::unique_ptr<IValueProvider>> &parameters) {
    auto ptr = dynamic_cast<PointerProvider *>(parameters[0].get());
    if (ptr == nullptr) {
      throw std::runtime_error("Parameter 0 of FILTER must be a pointer");
    }
    state = ptr->getAttributes()[0];
  };

  static RJValue calculate(
      const State &state,
      const std::vector<std::unique_ptr<IValueProvider>> &parameters,
      const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) {
    RJValue arr(rapidjson::kArrayType);

    // Get base array pointer
    RJValue base_array_;
    const RJValue* base_array = nullptr;
    if(parameters[0]->isAtom()){
      base_array_ = parameters[0]->getAtomValue(json, alloc);
      if(base_array_.IsArray()){
        base_array = &base_array_;
      }
    }else{
      base_array = parameters[0]->getValue(json,alloc);
    }

    if(base_array == nullptr) return RJValue();
    if(!base_array->IsArray()) {
      DLOG(WARNING) << "Base not array, should not be possible";
      return arr;
    }
    // Iterate array
    size_t i = 0;
    for(const auto& v : base_array->GetArray()){
      auto prefix = state + "/" + std::to_string(i);
      auto param = parameters[1]->duplicate();
      param->prependAttributes(prefix);
      // Check if filter predicate true
      if(param->getNonTruthyBool(json,alloc)){
        // Copy to output array
        RJValue copy(v,alloc,true);
        arr.PushBack(copy,alloc);
      }
      i++;
    }

    return arr;
  }
};

}  // namespace joda::query::providers::iterator

namespace joda::query {
template class TemplateProvider<
    joda::query::providers::iterator::AnyCalculator>;

typedef TemplateProvider<joda::query::providers::iterator::AnyCalculator>
    AnyProvider;

template class TemplateProvider<
    joda::query::providers::iterator::AllCalculator>;

typedef TemplateProvider<joda::query::providers::iterator::AllCalculator>
    AllProvider;

template class TemplateProvider<
    joda::query::providers::iterator::FilterCalculator>;

typedef TemplateProvider<joda::query::providers::iterator::FilterCalculator>
    FilterProvider;
template class TemplateProvider<
    joda::query::providers::iterator::MapCalculator>;

typedef TemplateProvider<joda::query::providers::iterator::MapCalculator>
    MapProvider;
}  // namespace joda::query

#endif  // JODA_ITERATORPROVIDER_H
