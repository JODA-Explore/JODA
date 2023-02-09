//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_CHOOSE_STATE_H
#define JODA_CHOOSE_STATE_H

#include "Query_State.h"

namespace joda::queryparsing::grammar {

struct chooseState {
  inline chooseState() {}

   std::unique_ptr<joda::query::IValueProvider> func = nullptr;

template <typename Input>
  inline void success(const Input &in, queryState &qs) {
    if(func == nullptr){
      func = std::make_unique<joda::query::BoolProvider>(true);
    }
    qs.q->setChoose(std::move(func));
  }



};
}  // namespace joda::queryparsing::grammar
#endif  // JODA_CHOOSE_STATE_H
