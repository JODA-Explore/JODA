//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_DELETE_ACTIONS_H
#define JODA_DELETE_ACTIONS_H
#include "../grammar/Grammar.h"
#include "../states/States.h"

namespace joda::queryparsing::grammar {
template <>
struct deleteAction<deleteIdent> {
  template <typename Input>
  static void apply(const Input &in, deleteState &state) {
    state.deleteVar = in.string();
  }
};
}  // namespace joda::queryparsing::grammar

#endif  // JODA_DELETE_ACTIONS_H
