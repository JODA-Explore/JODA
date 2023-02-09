//
// Created by Nico on 08/05/2019.
//

#include <joda/query/values/NullProvider.h>
#include <algorithm>
#include <string>

#ifndef JODA_FUNCTION_ACTIONS_H
#define JODA_FUNCTION_ACTIONS_H
namespace joda::queryparsing::grammar {

template <>
struct functionAction<unprefixed_pointer> {
  template <typename Input>
  static void apply(const Input &in, functionState &state) {
    std::string pointer = in.string();
    auto para = std::make_unique<joda::query::PointerProvider>(
        pointer.substr(1, pointer.size() - 2));
    if (state.atom == NO_ATOM) {
      state.atom = ATOM_POINTER;
    }
    state.params.push_back(std::move(para));
  }
};

template <>
struct functionAction<prefixed_pointer> {
  template <typename Input>
  static void apply(const Input &in, functionState &state) {
    std::string pointer = in.string();
    if(pointer.size() > 1){ // "$'...'" => "..."
      pointer =  pointer.substr(2, pointer.size() - 3);
    }else{
      pointer.clear(); // $ => ""
    }
    auto para = std::make_unique<joda::query::PointerProvider>(pointer, true);
    if (state.atom == NO_ATOM) {
      state.atom = ATOM_POINTER;
    }
    state.params.push_back(std::move(para));
  }
};

template <>
struct functionAction<floatNumber> {
  template <typename Input>
  static bool apply(const Input &in, functionState &state) {
    std::string str = in.string();

    std::unique_ptr<joda::query::IValueProvider> ival;

    try {
      double i = std::stod(str);
      ival = std::make_unique<joda::query::DoubleProvider>(i);
    } catch (std::exception &e) {
      LOG(ERROR) << "Could not parse number, out of range?";
      return false;
    }

    if (ival == nullptr) return false;

    auto para = std::move(ival);
    if (state.atom == NO_ATOM) {
      state.atom = ATOM_NUMBER;
    }
    state.params.push_back(std::move(para));
    return true;
  }
};

template <>
struct functionAction<intNumber> {
  template <typename Input>
  static bool apply(const Input &in, functionState &state) {
    std::string str = in.string();

    std::unique_ptr<joda::query::IValueProvider> ival;

    try {
      int64_t i = std::stol(str);
      ival = std::make_unique<joda::query::Int64Provider>(i);
    } catch (std::exception &e) {
    }

    if (ival == nullptr) {
      try {
        u_int64_t i = std::stoul(str);
        ival = std::make_unique<joda::query::UInt64Provider>(i);
      } catch (std::exception &e) {
      }
    }
    if (ival == nullptr) {
      try {
        double i = std::stod(str);
        ival = std::make_unique<joda::query::DoubleProvider>(i);
      } catch (std::exception &e) {
        LOG(ERROR) << "Could not parse number, out of range?";
        return false;
      }
    }

    if (ival == nullptr) return false;

    auto para = std::move(ival);
    if (state.atom == NO_ATOM) {
      state.atom = ATOM_NUMBER;
    }
    state.params.push_back(std::move(para));
    return true;
  }
};

template <>
struct functionAction<stringAtom> {
  static inline std::string unescape(const std::string &s) {
    std::string res;
    std::string::const_iterator it = s.begin();
    while (it != s.end()) {
      char c = *it++;
      if (c == '\\' && it != s.end()) {
        switch (*it++) {
          case '\\':
            c = '\\';
            break;
          case '"':
            c = '"';
            break;
            // all other escapes
          default:
            // invalid escape sequence - skip it. alternatively you can copy it
            // as is, throw an exception...
            continue;
        }
      }
      res += c;
    }

    return res;
  }
  template <typename Input>
  static void apply(const Input &in, functionState &state) {
    std::string str = in.string();
    // Unescape string
    auto unescaped = unescape(str.substr(1, str.size() - 2));
    auto para = std::make_unique<joda::query::StringProvider>(unescaped);
    if (state.atom == NO_ATOM) {
      state.atom = ATOM_STRING;
    }
    state.params.push_back(std::move(para));
  }
};
template <>
struct functionAction<boolAtom> {
  template <typename Input>
  static void apply(const Input &in, functionState &state) {
    std::string b = in.string();
    bool bo{};
    if (b == "true")
      bo = true;
    else if (b == "false")
      bo = false;
    else
      assert(false && "Did not change bool parsing");
    auto para = std::make_unique<joda::query::BoolProvider>(bo);
    if (state.atom == NO_ATOM) {
      state.atom = ATOM_BOOL;
    }
    state.params.push_back(std::move(para));
  }
};

template <>
struct functionAction<nullAtom> {
  template <typename Input>
  static void apply(const Input &in, functionState &state) {
    auto para = std::make_unique<joda::query::NullProvider>();
    if (state.atom == NO_ATOM) {
      state.atom = ATOM_NULL;
    }
    state.params.push_back(std::move(para));
  }
};

template <>
struct functionAction<negate> {
  static void apply0(BoolState &state) {
    state.negated = true;
  }
};

template <>
struct functionAction<gt> {
  static void apply0(CompareState &state) {
    state.less = false;
    state.comparison = true;
  }
};

template <>
struct functionAction<lt> {
  static void apply0(CompareState &state) {
    state.less = true;
    state.comparison = true;
  }
};

template <>
struct functionAction<and_equal> {
  static void apply0(CompareState &state) {
    state.equal = true;
  }
};

template <>
struct functionAction<unequal> {
  static void apply0(CompareState &state) {
    state.equal = false;
  }
};

template <>
struct functionAction<equal> {
  static void apply0(CompareState &state) {
    state.equal = true;
  }
};

// CUSTOM module
template <>
struct functionAction<func_kw_CUSTOM> {
  template <typename Input>
  static bool apply(const Input &in, functionState &state) {
    auto custom_funcs = joda::extension::ModuleRegister::getInstance().getIValFuncs();
    std::string custom_func = in.string();
    std::transform(custom_func.begin(), custom_func.end(),custom_func.begin(), ::toupper);
    if (std::ranges::find(custom_funcs,custom_func) == custom_funcs.end()) {
      return false;
    }
    state.factory = [custom_func](std::vector<std::unique_ptr<joda::query::IValueProvider>> && params){
      return joda::extension::ModuleRegister::getInstance().getIValFunc(custom_func,std::move(params));
    };
    return true;
  }
};

}  // namespace joda::queryparsing::grammar
#endif  // JODA_FUNCTION_ACTIONS_H
