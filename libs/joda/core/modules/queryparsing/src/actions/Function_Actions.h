//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_FUNCTION_ACTIONS_H
#define JODA_FUNCTION_ACTIONS_H
namespace joda::queryparsing::grammar {

template <>
struct functionAction<pointer> {
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

}  // namespace joda::queryparsing::grammar
#endif  // JODA_FUNCTION_ACTIONS_H
