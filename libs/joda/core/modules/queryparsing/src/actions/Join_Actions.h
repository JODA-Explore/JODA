

#ifndef JODA_JOIN_ACTIONS_H
#define JODA_JOIN_ACTIONS_H

#include "../grammar/Grammar.h"
#include "../states/States.h"

namespace joda::queryparsing::grammar {

template <>
struct joinAction<joinON_KW> {
  static void apply0(joinState &state) {
    state.type = JoinType::EqualityJoin;
  }
};

template <>
struct joinAction<joinWHERE_KW> {
  static void apply0(joinState &state) {
    state.type = JoinType::ThetaJoin;
  }
};

template <>
struct joinAction<loadIdent> {
  template <typename Input>
  static void apply(const Input &in, joinState &state) {
    state.outerIdent = in.string();
  }
};

}  // namespace joda::queryparsing::grammar
#endif  // JODA_JOIN_ACTIONS_H
