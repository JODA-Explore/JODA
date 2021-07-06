//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_FUNCTION_STATE_H
#define JODA_FUNCTION_STATE_H

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

struct functionState {
  /*
   * LOAD Master
   */
  template <typename Input>
  inline functionState(const Input &in, loadState &qs) {}

  template <typename Input>
  inline void success(const Input &in, loadState &qs) {
    bool b = false;
    auto prov = createFunc(in);
    assert(prov != nullptr && "May not be null");
    b = qs.putValProv(std::move(prov));
    assert(b && "State should not be full");
  }

  /*
   * STORE Master
   */
  template <typename Input>
  inline functionState(const Input &in, storeState &qs) {}

  template <typename Input>
  inline void success(const Input &in, storeState &qs) {
    bool b = false;
    auto prov = createFunc(in);
    assert(prov != nullptr && "May not be null");
    b = qs.putValProv(std::move(prov));
    assert(b && "State should not be full");
  }

  /*
   * Choose Master
   */
  template <typename Input>
  inline functionState(const Input &in, chooseState &qs) {}

  template <typename Input>
  inline void success(const Input &in, chooseState &qs) {
    bool b = false;
    auto prov = createFunc(in);
    assert(prov != nullptr && "May not be null");
    b = qs.putValProv(std::move(prov));
    assert(b && "State should not be full");
  }

  /*
   * AS Master
   */
  template <typename Input>
  inline functionState(const Input &in, asState &qs) {}

  template <typename Input>
  inline void success(const Input &in, asState &qs) {
    bool b = false;
    auto prov = createFunc(in);
    assert(prov != nullptr && "May not be null");
    b = qs.putValProv(std::move(prov));
    assert(b && "State should not be full");
  }

  /*
   * AGG Master
   */
  template <typename Input>
  inline functionState(const Input &in, aggState &qs) {}

  template <typename Input>
  inline void success(const Input &in, aggState &qs) {
    bool b = false;
    auto prov = createFunc(in);
    assert(prov != nullptr && "May not be null");
    b = qs.putValProv(std::move(prov));
    assert(b && "State should not be full");
  }
  /*
   * Function Master
   */
  template <typename Input>
  inline functionState(const Input &in, functionState &qs) {}

  template <typename Input>
  inline void success(const Input &in, functionState &qs) {
    bool b = false;
    auto prov = createFunc(in);
    assert(prov != nullptr && "May not be null");
    b = qs.putValProv(std::move(prov));
    assert(b && "State should not be full");
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

  Atom_Value atom = NO_ATOM;
  std::function<std::unique_ptr<joda::query::IValueProvider>(
      std::vector<std::unique_ptr<joda::query::IValueProvider>> &&)>
      factory = nullptr;
  std::vector<std::unique_ptr<joda::query::IValueProvider>> params;
};
}  // namespace joda::queryparsing::grammar
#endif  // JODA_FUNCTION_STATE_H
