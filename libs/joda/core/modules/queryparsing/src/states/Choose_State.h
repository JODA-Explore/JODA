//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_CHOOSE_STATE_H
#define JODA_CHOOSE_STATE_H

#include "Query_State.h"
#include <joda/query/predicate/ValToPredicate.h>

namespace joda::queryparsing::grammar {

struct chooseState {
  template<typename Input>
  inline chooseState(const Input &in, queryState &qs) {
    comp = NONE;
  }

  template<typename Input>
  inline void success(const Input &in, queryState &qs) {
    assert(valProv.first == nullptr);
    assert(valProv.second == nullptr);
    assert(ptr.empty());
    if (preds.empty()) {
      std::unique_ptr<joda::query::Predicate> pred  = std::make_unique<joda::query::ValToPredicate>(true);
      qs.q->setPredicate(std::move(pred));
      return;
    }

    if (preds.top().second.empty()) {
      std::unique_ptr<joda::query::Predicate> t = std::make_unique<joda::query::ValToPredicate>(true);
      preds.top().second.push_back(std::move(t));
    }
    assert(preds.top().second.size() == 1);
    std::unique_ptr<joda::query::Predicate>  tmp =std::move(preds.top().second[0]);
    qs.q->setPredicate(std::move(tmp));
  }

  inline bool putValProv(std::unique_ptr<joda::query::IValueProvider> &&val) {
    assert(val != nullptr && "Should not pass nullptr");
    if (val == nullptr) return false;
    if (valProv.first == nullptr) {
      valProv.first = std::move(val);
      return true;
    }
    if (valProv.second == nullptr) {
      valProv.second = std::move(val);
      return true;
    }
    assert(false && "Should not be full");
    return false;
  }

  predStack preds{};
  std::pair<std::unique_ptr<joda::query::IValueProvider>, std::unique_ptr<joda::query::IValueProvider>> valProv;
  Comparison comp;
  std::string ptr;
};
}
#endif //JODA_CHOOSE_STATE_H
