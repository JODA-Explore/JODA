
#ifndef JODA_CASTPROVIDER_H
#define JODA_CASTPROVIDER_H

#include "IValueProvider.h"
#include "ParameterPack.h"
#include "TemplateProvider.h"


 
  //  _____ _   _ _______ 
  // |_   _| \ | |__   __|
  //   | | |  \| |  | |   
  //   | | | . ` |  | |   
  //  _| |_| |\  |  | |   
  // |_____|_| \_|  |_|   
                       
                       
 

namespace joda::query::providers::cast {

class IntCastHandler {
 public:
  typedef char Ch;

  bool Null() { return false; }

  bool Bool(bool b) {
    if (b)
      val = RJValue(1);
    else
      val = RJValue(0);
    return false;
  }

  bool Int(int i) {
    val = RJValue(i);
    return false;
  }

  bool Uint(unsigned i) {
    val = RJValue(i);
    return false;
  }

  bool Int64(int64_t i) {
    val = RJValue(i);
    return false;
  }

  bool Uint64(uint64_t i) {
    val = RJValue(i);
    return false;
  }

  bool Double(double d) {
    val = RJValue((int64_t)d);
    return false;
  }

  bool RawNumber(const Ch *str, rapidjson::SizeType length, bool copy) {
    return false;
  }

  bool String(const Ch *str, rapidjson::SizeType length, bool copy) {
    try {
      val = RJValue((int64_t)std::stoll({str, length}));
    } catch (std::exception e) {
      val = RJValue();
    }

    return false;
  }

  bool StartObject() { return false; }

  bool Key(const Ch *str, rapidjson::SizeType length, bool copy) {
    return false;
  }

  bool EndObject(rapidjson::SizeType memberCount) { return false; }

  constexpr bool StartArray() { return false; }

  constexpr bool EndArray(rapidjson::SizeType elementCount) { return false; }
  RJValue getValue() { return std::move(val); }

 private:
  RJValue val;
};

class IntCastCalculator {
 public:
  // Parameters
  typedef values::AnyParameter<0> P0;
  typedef values::NoParameter<1> P1;
  typedef values::NoParameter<2> P2;
  typedef values::NoParameter<3> P3;
  typedef values::NoParameter<4> P4;

  // State
  typedef bool State;

  // Return value of the function
  static constexpr IValueType retType = IV_Number;

  static constexpr auto name = "INT";
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
    IntCastHandler handler;
    accepter.Accept(json, alloc, handler);
    return handler.getValue();
  };
};
}  // namespace joda::query::providers::cast

namespace joda::query {
template class TemplateProvider<
    joda::query::providers::cast::IntCastCalculator>;

typedef TemplateProvider<joda::query::providers::cast::IntCastCalculator>
    IntCastProvider;
}  // namespace joda::query



 
  //  ______ _      ____       _______ 
  // |  ____| |    / __ \   /\|__   __|
  // | |__  | |   | |  | | /  \  | |   
  // |  __| | |   | |  | |/ /\ \ | |   
  // | |    | |___| |__| / ____ \| |   
  // |_|    |______\____/_/    \_\_|   
                                    
                                    
 


namespace joda::query::providers::cast {
class FloatCastHandler {
 public:
  typedef char Ch;

  bool Null() { return false; }

  bool Bool(bool b) {
    if (b)
      val = RJValue(1.0);
    else
      val = RJValue(0.0);
    return false;
  }

  bool Int(int i) {
    val = RJValue((double)i);
    return false;
  }

  bool Uint(unsigned i) {
    val = RJValue((double)i);
    return false;
  }

  bool Int64(int64_t i) {
    val = RJValue((double)i);
    return false;
  }

  bool Uint64(uint64_t i) {
    val = RJValue((double)i);
    return false;
  }

  bool Double(double d) {
    val = RJValue(d);
    return false;
  }

  bool RawNumber(const Ch *str, rapidjson::SizeType length, bool copy) {
    return false;
  }

  bool String(const Ch *str, rapidjson::SizeType length, bool copy) {
    try {
      val = RJValue(std::stod({str, length}));
    } catch (std::exception e) {
      val = RJValue();
    }

    return false;
  }

  bool StartObject() { return false; }

  bool Key(const Ch *str, rapidjson::SizeType length, bool copy) {
    return false;
  }

  bool EndObject(rapidjson::SizeType memberCount) { return false; }

  constexpr bool StartArray() { return false; }

  constexpr bool EndArray(rapidjson::SizeType elementCount) { return false; }
  RJValue getValue() { return std::move(val); }

 private:
  RJValue val;
};

class FloatCastCalculator {
 public:
  // Parameters
  typedef values::AnyParameter<0> P0;
  typedef values::NoParameter<1> P1;
  typedef values::NoParameter<2> P2;
  typedef values::NoParameter<3> P3;
  typedef values::NoParameter<4> P4;

  // State
  typedef bool State;

  // Return value of the function
  static constexpr IValueType retType = IV_Number;

  static constexpr auto name = "FLOAT";
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
    FloatCastHandler handler;
    accepter.Accept(json, alloc, handler);
    return handler.getValue();
  };
};

}  // namespace joda::query::providers::cast

namespace joda::query {
template class TemplateProvider<
    joda::query::providers::cast::FloatCastCalculator>;

typedef TemplateProvider<joda::query::providers::cast::FloatCastCalculator>
    FloatCastProvider;
}  // namespace joda::query


 
  //   _____ _______ _____  _____ _   _  _____ 
  //  / ____|__   __|  __ \|_   _| \ | |/ ____|
  // | (___    | |  | |__) | | | |  \| | |  __ 
  //  \___ \   | |  |  _  /  | | | . ` | | |_ |
  //  ____) |  | |  | | \ \ _| |_| |\  | |__| |
  // |_____/   |_|  |_|  \_\_____|_| \_|\_____|
                                            
                                            
 



namespace joda::query::providers::cast {
class StringCastHandler {
 public:
  StringCastHandler(RJMemoryPoolAlloc* alloc): alloc(alloc){}

  typedef char Ch;

  bool Null() {
    val = RJValue("null",*alloc);
    return false; 
  }

  bool Bool(bool b) {
    if (b)
      val = RJValue("true",*alloc);
    else
      val = RJValue("false",*alloc);
    return false;
  }

  bool Int(int i) {
    val = RJValue(std::to_string(i),*alloc);
    return false;
  }

  bool Uint(unsigned i) {
    val = RJValue(std::to_string(i),*alloc);
    return false;
  }

  bool Int64(int64_t i) {
    val = RJValue(std::to_string(i),*alloc);
    return false;
  }

  bool Uint64(uint64_t i) {
    val = RJValue(std::to_string(i),*alloc);
    return false;
  }

  bool Double(double d) {
    val = RJValue(std::to_string(d),*alloc);
    return false;
  }

  bool RawNumber(const Ch *str, rapidjson::SizeType length, bool copy) {
     val = RJValue(str,*alloc);
    return false;
  }

  bool String(const Ch *str, rapidjson::SizeType length, bool copy) {
    val = RJValue(str,*alloc);
    return false;
  }

  bool StartObject() { return false; }

  bool Key(const Ch *str, rapidjson::SizeType length, bool copy) {
    return false;
  }

  bool EndObject(rapidjson::SizeType memberCount) { return false; }

  constexpr bool StartArray() { return false; }

  constexpr bool EndArray(rapidjson::SizeType elementCount) { return false; }
  RJValue getValue() { return std::move(val); }

 private:
  RJMemoryPoolAlloc* alloc;
  RJValue val;
};

class StringCastCalculator {
 public:
  // Parameters
  typedef values::AnyParameter<0> P0;
  typedef values::NoParameter<1> P1;
  typedef values::NoParameter<2> P2;
  typedef values::NoParameter<3> P3;
  typedef values::NoParameter<4> P4;

  // State
  typedef bool State;

  // Return value of the function
  static constexpr IValueType retType = IV_Number;

  static constexpr auto name = "STRING";
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
    StringCastHandler handler(&alloc);
    accepter.Accept(json, alloc, handler);
    return handler.getValue();
  };
};

}  // namespace joda::query::providers::cast

namespace joda::query {
template class TemplateProvider<
    joda::query::providers::cast::StringCastCalculator>;

typedef TemplateProvider<joda::query::providers::cast::StringCastCalculator>
    StringCastProvider;
}  // namespace joda::query
#endif  // JODA_CASTPROVIDER_H
