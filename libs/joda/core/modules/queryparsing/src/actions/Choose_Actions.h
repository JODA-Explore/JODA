//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_CHOOSE_ACTIONS_H
#define JODA_CHOOSE_ACTIONS_H

#include "../grammar/Grammar.h"
#include "../states/States.h"


namespace joda::queryparsing::grammar {

  
template <>
struct chooseExpAction<predicate_expression> {
  static bool apply0(chooseState &state) {
    if(state.func == nullptr) return false;
    if(state.func->isBool() || state.func->isAny()){
      return true;
    }
    return false;
  }
};
}  // namespace joda::queryparsing::grammar
#endif  // JODA_CHOOSE_ACTIONS_H
