//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_CHOOSE_ACTIONS_H
#define JODA_CHOOSE_ACTIONS_H

#include "../grammar/Grammar.h"
#include "../states/States.h"

#include <joda/query/values/AtomProvider.h>
#include <joda/query/values/NullProvider.h>
#include "joda/query/values/IsXBoolProvider.h"
#include "joda/query/values/PointerProvider.h"

namespace joda::queryparsing::grammar {

template<>
struct chooseExpAction<pointer> {
  template<typename Input>
  static void apply(const Input &in,
                    chooseState &state) {
    if (state.valProv.first == nullptr) {
      std::string pointer = in.string();
      state.valProv.first = std::move(std::make_unique<joda::query::PointerProvider>(pointer.substr(1, pointer.size() - 2)));
      return;
    }
    if (state.valProv.second == nullptr) {
      std::string pointer = in.string();
      state.valProv.second = std::move(std::make_unique<joda::query::PointerProvider>(pointer.substr(1, pointer.size() - 2)));
      return;
    }
    assert(false);
  }
};
template<>
struct chooseExpAction<stringAtom> {
  static inline std::string unescape(const std::string& s)
  {
    std::string res;
    std::string::const_iterator it = s.begin();
    while (it != s.end())
    {
      char c = *it++;
      if (c == '\\' && it != s.end())
      {
        switch (*it++) {
          case '\\': c = '\\'; break;
          case '"': c = '"'; break;
            // all other escapes
          default:
            // invalid escape sequence - skip it. alternatively you can copy it as is, throw an exception...
            continue;
        }
      }
      res += c;
    }

    return res;
  }
  template<typename Input>
  static void apply(const Input &in,
                    chooseState &state) {
    std::string str = in.string();
    //Unescape string
    auto unescaped = unescape(str.substr(1, str.size() - 2));

    if (state.valProv.first == nullptr) {
      state.valProv.first = std::move(std::make_unique<joda::query::StringProvider>(unescaped));
      return;
    }
    if (state.valProv.second == nullptr) {
      state.valProv.second = std::move(std::make_unique<joda::query::StringProvider>(unescaped));
      return;
    }
    assert(false);
  }
};
template<>
struct chooseExpAction<boolAtom> {
  template<typename Input>
  static void apply(const Input &in,
                    chooseState &state) {
    std::string str = in.string();
    bool b = str == "true";
    if (!b) { //Not "true"
      b = str == "false";
      if (!b) assert(false);//Also not false => Error
      else b = false; //Is "False" so b = false
    }

    if (state.valProv.first == nullptr) {
      state.valProv.first = std::move(std::make_unique<joda::query::BoolProvider>(b));
      return;
    }
    if (state.valProv.second == nullptr) {
      state.valProv.second = std::move(std::make_unique<joda::query::BoolProvider>(b));
      return;
    }
    assert(false);
  }
};
template<>
struct chooseExpAction<numberAtom> {
  template<typename Input>
  static bool apply(const Input &in,
                    chooseState &state) {
    std::string str = in.string();

    char *end;
    auto s = str.c_str();

    std::unique_ptr<joda::query::IValueProvider> ival;

    bool try_unisgned = false;
    bool try_double = false;

    try {
      int64_t i = std::stol(str);
      ival = std::move(std::make_unique<joda::query::Int64Provider>(i));
    } catch (std::out_of_range &e) {
      try_unisgned = true;
    } catch (std::exception &e) {
      try_double = true;
    }

    if (try_unisgned) {
      try {
        u_int64_t i = std::stoul(str);
        ival = std::move(std::make_unique<joda::query::UInt64Provider>(i));
      } catch (std::exception &e) {
        try_double = true;
      }
    }

    try {
      double i = std::stod(str);
      ival = std::move(std::make_unique<joda::query::DoubleProvider>(i));
    } catch (std::exception &e) {
      return false;
    }

    DCHECK(ival != nullptr);

    if (state.valProv.first == nullptr) {
      state.valProv.first = std::move(ival);
      return true;
    }
    if (state.valProv.second == nullptr) {
      state.valProv.second = std::move(ival);
      return true;
    }
    return false;
  }
};
template<>
struct chooseExpAction<gt> {
  template<typename Input>
  static void apply(const Input &in,
                    chooseState &state) {
    std::string str = in.string();

    if (str.size() == 1) {
      state.comp = GT;
      return;
    }
    if (str.size() == 2) {
      state.comp = GTE;
      return;
    }
    assert(false);
  }
};

template<>
struct chooseExpAction<lt> {
  template<typename Input>
  static void apply(const Input &in,
                    chooseState &state) {
    std::string str = in.string();

    if (str.size() == 1) {
      state.comp = LT;
      return;
    }
    if (str.size() == 2) {
      state.comp = LTE;
      return;
    }
    assert(false);
  }
};

template<>
struct chooseExpAction<equal> {
  template<typename Input>
  static void apply(const Input &in,
                    chooseState &state) {
    std::string str = in.string();

    assert(str.size() == 2);
    if (str[0] == '!') {
      state.comp = NEQU;
      return;
    }
    if (str[0] == '=') {
      state.comp = EQU;
      return;
    }
    assert(false);
  }
};

template<>
struct chooseExpAction<compareExp> {
  template<typename Input>
  static bool apply(const Input &in,
                    chooseState &state) {
    if (state.valProv.first == nullptr){
      assert(false && "There has to be at least one value");
      return false; //May not happen that
    }

    std::unique_ptr<joda::query::Predicate> pred = nullptr;
    try {
      if (state.valProv.second == nullptr) {//Only one value => ValToPredicate
        assert(state.comp == NONE && "If not 'NONE' something went wrong");
        pred = std::make_unique<joda::query::ValToPredicate>(std::move(state.valProv.first));
      } else {
        try {
          switch (state.comp) {
            case GT:
              pred = std::make_unique<joda::query::ComparePredicate>(std::move(state.valProv.first),
                                                                     std::move(state.valProv.second),
                                                                     true,
                                                                     false);
              break;
            case GTE:
              pred = std::make_unique<joda::query::ComparePredicate>(std::move(state.valProv.first),
                                                                     std::move(state.valProv.second),
                                                                     true,
                                                                     true);
              break;
            case LT:
              pred = std::make_unique<joda::query::ComparePredicate>(std::move(state.valProv.first),
                                                                     std::move(state.valProv.second),
                                                                     false,
                                                                     false);
              break;
            case LTE:
              pred = std::make_unique<joda::query::ComparePredicate>(std::move(state.valProv.first),
                                                                     std::move(state.valProv.second),
                                                                     false,
                                                                     true);
              break;
            case EQU:
              pred = std::make_unique<joda::query::EqualizePredicate>(std::move(state.valProv.first),
                                                                      std::move(state.valProv.second),
                                                                      true);
              break;
            case NEQU:
              pred = std::make_unique<joda::query::EqualizePredicate>(std::move(state.valProv.first),
                                                                      std::move(state.valProv.second),
                                                                      false);
              break;
            case NONE:assert(false && "May not happen");
              return false;
              break;
          }
          state.comp = NONE;
          state.valProv.first = nullptr;
          state.valProv.second = nullptr;

        } catch (const joda::query::NotComparableException &e) {
          throw tao::pegtl::parse_error("These values cannot be compared. ", in);
        } catch (const joda::query::NotEqualizableException &e) {
          throw tao::pegtl::parse_error("These values cannot be compared for (un)equality.", in);
        };
      }
    } catch (const joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
    assert(pred != nullptr);

    state.preds.top().second.push_back(std::move(pred));
    return true;
  }
};

template<>
struct chooseExpAction<unaryExp> {
  template<typename Input>
  static void apply(const Input &in,
                    chooseState &state) {

    std::string str = in.string();

    auto it = std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun(::isspace)));
    if (it != str.end()) {
      if (*it == '!') { //SHould it be negated?
        //Negate last Pred
        assert(state.preds.top().second.back() != nullptr);
        auto tmp = std::make_unique<joda::query::NegatePredicate>(std::move(state.preds.top().second.back()));
        state.preds.top().second.back() = std::move(tmp);
      }
    }
  }
};

template<>
struct chooseExpAction<andExp> {
  template<typename Input>
  static void apply(const Input &in,
                    chooseState &state) {
    assert(!state.preds.empty());
    assert(state.preds.top().first == AND);
    auto pair = std::move(state.preds.top());
    state.preds.pop();
    assert(!pair.second.empty());
    std::unique_ptr<joda::query::Predicate> tmpPtr = nullptr;
    if (pair.second.size() > 1) {
      for (int i = pair.second.size() - 1; i >= 0; --i) {
        if (tmpPtr == nullptr)
          tmpPtr = std::move(pair.second[i]);
        else {
          auto andPtr = std::make_unique<joda::query::AndPredicate>(std::move(pair.second[i]), std::move(tmpPtr));
          tmpPtr = std::move(andPtr);
        }
      }
    } else tmpPtr = std::move(pair.second.front());
    assert(!state.preds.empty());
    state.preds.top().second.push_back(std::move(tmpPtr));
  }
};
template<>
struct chooseExpAction<orExp> {
  template<typename Input>
  static void apply(const Input &in,
                    chooseState &state) {
    assert(!state.preds.empty());
    assert(state.preds.top().first == OR);
    auto pair = std::move(state.preds.top());
    state.preds.pop();
    assert(!pair.second.empty());
    std::unique_ptr<joda::query::Predicate> tmpPtr = nullptr;
    if (pair.second.size() > 1) {
      for (int i = pair.second.size() - 1; i >= 0; --i) {
        if (tmpPtr == nullptr)
          tmpPtr = std::move(pair.second[i]);
        else {
          auto andPtr = std::make_unique<joda::query::OrPredicate>(std::move(pair.second[i]), std::move(tmpPtr));
          tmpPtr = std::move(andPtr);
        }
      }
    } else tmpPtr = std::move(pair.second.front());
    assert(!state.preds.empty());
    state.preds.top().second.push_back(std::move(tmpPtr));
  }
};

template<>
struct chooseExpAction<qexp> {
  template<typename Input>
  static void apply(const Input &in,
                    chooseState &state) {

  }
};
template<>
struct chooseExpAction<andStart> {
  template<typename Input>
  static void apply(const Input &in,
                    chooseState &state) {
    state.preds.emplace(std::make_pair(AND, std::vector<std::unique_ptr<joda::query::Predicate>>()));
  }
};
template<>
struct chooseExpAction<orStart> {
  template<typename Input>
  static void apply(const Input &in,
                    chooseState &state) {
    state.preds.emplace(std::make_pair(OR, std::vector<std::unique_ptr<joda::query::Predicate>>()));
  }
};
template<>
struct chooseExpAction<expStart> {
  template<typename Input>
  static void apply(const Input &in,
                    chooseState &state) {

    state.preds.emplace(std::make_pair(BASE, std::vector<std::unique_ptr<joda::query::Predicate>>()));
  }
};
}
#endif //JODA_CHOOSE_ACTIONS_H
