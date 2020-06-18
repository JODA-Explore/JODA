//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_AS_ACTIONS_H
#define JODA_AS_ACTIONS_H

#include "../grammar/Grammar.h"
#include "../states/States.h"

#include "joda/query/predicate/ValToPredicate.h"
#include "joda/query/project/FlattenProjector.h"
#include "joda/query/project/ValueProviderProjector.h"
namespace joda::queryparsing::grammar {

template<>
struct asExpAction<functionstateaction> {
  static void apply0(asState &state) {
    if(state.fun == NOPROJ)
      state.fun = valProvProj;
  }
};
template<>
struct asExpAction<projectKW_ARRFLAT> {
  static void apply0(asState &state) {
    assert(state.fun == NOPROJ);
    state.fun = arrFlatProj;
  }
};

template<>
struct asExpAction<projectFromPointer> {
  template<typename Input>
  static void apply(const Input &in,
                    asState &state) {
    assert(state.fun == NOPROJ || state.fun == arrFlatProj);
    if (state.fun == NOPROJ) state.fun = valProvProj;
    std::string pointer = in.string();
    state.putValProv(std::make_unique<joda::query::PointerProvider>(pointer.substr(1, pointer.size() - 2)));
  }
};

template<>
struct asExpAction<projectToPointer> {
  template<typename Input>
  static void apply(const Input &in,
                    asState &state) {

    assert(state.toPointer.empty());
    std::string pointer = in.string();
    state.toPointer = pointer.substr(1, pointer.size() - 2);
  }
};
template<>
struct asExpAction<projectSingleExp> {
  static void apply0(asState &state) {
    assert(state.fun != NOPROJ && state.valprov != nullptr);
    switch (state.fun) {
      case NOPROJ: assert(false);
        break;
      case arrFlatProj:
        state.setprojs.push_back(std::make_unique<joda::query::FlattenProjector>(state.toPointer,
                                                                                 std::move(state.valprov)));
        break;
      case valProvProj:assert(state.valprov != nullptr);
        state.projs.push_back(std::make_unique<joda::query::ValueProviderProjector>(state.toPointer, std::move(state.valprov)));
        break;
    }
    state.toPointer.clear();
    state.fun = NOPROJ;
    assert(state.toPointer.empty());
    assert(state.fun == NOPROJ);
    assert(state.valprov == nullptr);
  }
};

}
#endif //JODA_AS_ACTIONS_H
