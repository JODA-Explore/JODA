//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_FUNCTION_STATE_H
#define JODA_FUNCTION_STATE_H

#include <joda/query/values/BinaryBoolProvider.h>

#include "Join_State.h"
#include "Load_State.h"
#include "Query_State.h"
#include "Store_State.h"

namespace joda::queryparsing::grammar {
enum Atom_Value {
  NO_ATOM,
  ATOM_STRING,
  ATOM_NUMBER,
  ATOM_BOOL,
  ATOM_POINTER,
  ATOM_NULL
};

struct ValueState {
  std::unique_ptr<joda::query::IValueProvider> func = nullptr;

  inline ValueState(){};

  void addFunc(std::unique_ptr<joda::query::IValueProvider> &&f) {
    if (f == nullptr) return;
    DCHECK(func == nullptr);
    func = std::move(f);
  }

  template <typename Input>
  inline void success(const Input &in, chooseState &qs) {
    try {
      qs.func = std::move(func);
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }

  template <typename Input>
  inline void success(const Input &in, joinState &js) {
    try {
      js.addFunc(std::move(func));
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }
};

struct BoolState;  // Forward declaration

struct OrState {
  std::unique_ptr<joda::query::IValueProvider> func = nullptr;

  inline OrState(){};

  void addFunc(std::unique_ptr<joda::query::IValueProvider> &&f) {
    if (f == nullptr) return;
    if (func == nullptr) {
      func = std::move(f);
    } else {
      std::vector<std::unique_ptr<joda::query::IValueProvider>> params;
      params.emplace_back(std::move(func));
      params.emplace_back(std::move(f));
      func = std::make_unique<joda::query::OrProvider>(std::move(params));
    }
  }

  template <typename Input>
  inline void success(const Input &in, ValueState &qs) {
    try {
      qs.addFunc(std::move(func));
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }

  template <typename Input, typename State>
  inline void success(const Input &in, State &qs) {
    try {
      qs.addFunc(std::move(func));
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }
};

struct AndState {
  std::unique_ptr<joda::query::IValueProvider> func = nullptr;

  inline AndState(){};

  void addFunc(std::unique_ptr<joda::query::IValueProvider> &&f) {
    if (f == nullptr) return;
    if (func == nullptr) {
      func = std::move(f);
    } else {
      std::vector<std::unique_ptr<joda::query::IValueProvider>> params;
      params.emplace_back(std::move(func));
      params.emplace_back(std::move(f));
      func = std::make_unique<joda::query::AndProvider>(std::move(params));
    }
  }

  template <typename Input>
  inline void success(const Input &in, OrState &qs) {
    try {
      qs.addFunc(std::move(func));
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }
};

struct BoolState {
  std::unique_ptr<joda::query::IValueProvider> func = nullptr;
  bool negated = false;

  inline BoolState(){};

  void addFunc(std::unique_ptr<joda::query::IValueProvider> &&f) {
    if (f == nullptr) return;
    DCHECK(func == nullptr);
    func = std::move(f);
  }

  std::unique_ptr<joda::query::IValueProvider> getFunc() {
    if (!negated) {
      return std::move(func);
    }
    std::vector<std::unique_ptr<joda::query::IValueProvider>> params;
    params.emplace_back(std::move(func));
    return std::make_unique<joda::query::NotProvider>(std::move(params));
  }

  template <typename Input>
  inline void success(const Input &in, AndState &qs) {
    try {
      qs.addFunc(getFunc());
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }
};

struct CompareState {
  std::unique_ptr<joda::query::IValueProvider> func = nullptr;

  bool less = true;
  bool equal = false;
  bool comparison = false;

  inline CompareState(){};

  void addFunc(std::unique_ptr<joda::query::IValueProvider> &&f) {
    if (f == nullptr) return;
    if (func == nullptr) {  // LHS
      func = std::move(f);
    } else {  // RHS
      std::vector<std::unique_ptr<joda::query::IValueProvider>> params;
      params.emplace_back(std::move(func));
      params.emplace_back(std::move(f));

      if (comparison) {  //>,<,>=,<=
        if (less) {      // <, <=
          if (equal) {   // <=
            func = std::make_unique<joda::query::LessEqualProvider>(
                std::move(params));
          } else {  // <
            func =
                std::make_unique<joda::query::LessProvider>(std::move(params));
          }
        } else {        // >, >=
          if (equal) {  // >=
            func = std::make_unique<joda::query::GreaterEqualProvider>(
                std::move(params));
          } else {  // >
            func = std::make_unique<joda::query::GreaterProvider>(
                std::move(params));
          }
        }
      } else {        // ==,!=
        if (equal) {  // ==
          func =
              std::make_unique<joda::query::EqualProvider>(std::move(params));
        } else {  // . !=
          func =
              std::make_unique<joda::query::UnequalProvider>(std::move(params));
        }
      }
    }
  }

  template <typename Input>
  inline void success(const Input &in, BoolState &qs) {
    try {
      qs.addFunc(std::move(func));
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }
};

struct functionState {
  /*
   * LOAD Master
   */
  template <typename Input>
  inline functionState(const Input &in, loadState &qs) {}

  template <typename Input>
  inline void success(const Input &in, loadState &qs) {
    try {
      bool b = false;
      auto prov = createFunc(in);
      assert(prov != nullptr && "May not be null");
      b = qs.putValProv(std::move(prov));
      assert(b && "State should not be full");
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }

  /*
   * STORE Master
   */
  template <typename Input>
  inline functionState(const Input &in, storeState &qs) {}

  template <typename Input>
  inline void success(const Input &in, storeState &qs) {
    try {
      bool b = false;
      auto prov = createFunc(in);
      assert(prov != nullptr && "May not be null");
      b = qs.putValProv(std::move(prov));
      assert(b && "State should not be full");
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }

  /*
   * Choose Master
   */
  template <typename Input>
  inline functionState(const Input &in, chooseState &qs) {}

  template <typename Input>
  inline void success(const Input &in, chooseState &qs) {
    try {
      auto prov = createFunc(in);
      qs.func = std::move(prov);
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }

  /*
   * AS Master
   */
  template <typename Input>
  inline functionState(const Input &in, asState &qs) {}

  template <typename Input>
  inline void success(const Input &in, asState &qs) {
    try {
      bool b = false;
      auto prov = createFunc(in);
      assert(prov != nullptr && "May not be null");
      b = qs.putValProv(std::move(prov));
      assert(b && "State should not be full");
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }

  /*
   * AGG Master
   */
  template <typename Input>
  inline functionState(const Input &in, aggState &qs) {}

  template <typename Input>
  inline void success(const Input &in, aggState &qs) {
    try {
      bool b = false;
      auto prov = createFunc(in);
      assert(prov != nullptr && "May not be null");
      b = qs.putValProv(std::move(prov));
      assert(b && "State should not be full");
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }

  /*
   * JOIN Master
   */
  template <typename Input>
  inline functionState(const Input &in, joinState &qs) {}

  template <typename Input>
  inline void success(const Input &in, joinState &qs) {
    try {
      auto prov = createFunc(in);
      assert(prov != nullptr && "May not be null");
      if (qs.lhs == nullptr) {
        qs.lhs = std::move(prov);
      }
      if (qs.rhs == nullptr) {
        qs.rhs = std::move(prov);
      } else {
        CHECK(false) << "State should not be full";
      }
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }

  /*
   * Function Master
   */
  template <typename Input>
  inline functionState(const Input &in, functionState &qs) {}

  template <typename Input>
  inline void success(const Input &in, functionState &qs) {
    try {
      bool b = false;
      auto prov = createFunc(in);
      assert(prov != nullptr && "May not be null");
      b = qs.putValProv(std::move(prov));
      assert(b && "State should not be full");
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }

  /*
   * Comparison Master
   */
  template <typename Input>
  inline functionState(const Input &in, CompareState &qs) {}

  template <typename Input>
  inline void success(const Input &in, CompareState &qs) {
    try {
      auto prov = createFunc(in);
      qs.addFunc(std::move(prov));
    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    } catch (const joda::query::NotEqualizableException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    } catch (const joda::query::NotComparableException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }

  inline bool putValProv(std::unique_ptr<joda::query::IValueProvider> &&val) {
    assert(val != nullptr && "Should not pass nullptr");
    if (val == nullptr) return false;
    params.push_back(std::move(val));
    return true;
  }

  template <typename Input>
  inline std::unique_ptr<joda::query::IValueProvider> createFunc(
      const Input &in) {
    std::unique_ptr<joda::query::IValueProvider> prov = nullptr;
    try {
      if (factory != nullptr) {
        prov = factory(std::move(params));
        DCHECK(prov != nullptr)
            << "prov should not be null, except for thrown parsing exceptions";
      } else if (atom != NO_ATOM) {
        assert(params.size() == 1);
        prov = std::move(params.front());
      } else {
        DCHECK(false)
            << "There should always be either a atom function or factory";
        throw tao::pegtl::parse_error("Unexpected error during query parsing",
                                      in);
      }

    } catch (joda::query::WrongParameterException &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
    return prov;
  }

  void addFunc(std::unique_ptr<joda::query::IValueProvider> &&f) {
    if (f == nullptr) return;
    params.emplace_back(std::move(f));
  }

  Atom_Value atom = NO_ATOM;
  std::function<std::unique_ptr<joda::query::IValueProvider>(
      std::vector<std::unique_ptr<joda::query::IValueProvider>> &&)>
      factory = nullptr;
  std::vector<std::unique_ptr<joda::query::IValueProvider>> params;
};

}  // namespace joda::queryparsing::grammar
#endif  // JODA_FUNCTION_STATE_H
