//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_AGG_STATE_H
#define JODA_AGG_STATE_H
#include "Query_State.h"

namespace joda::queryparsing::grammar {

enum aggFunction { NOAGG, AVG, COUNT, SUM, ATTSTAT, DISTINCT, MIN, MAX, COLLECT };

struct aggState {
  template<typename Input>
  inline aggState(const Input &in, queryState &qs) {
    aggfun = NOAGG;
  }

  template<typename Input>
  inline void success(const Input &in, queryState &qs) {
    for (auto &agg : aggs) {
      qs.q->addAggregator(std::move(agg));
    }

  }

  inline bool putValProv(std::unique_ptr<joda::query::IValueProvider> &&val) {
    assert(val != nullptr && "Should not pass nullptr");
    if (val == nullptr) return false;
    valprov.push_back(std::move(val));
    return true;
  }

  std::vector<std::unique_ptr<joda::query::IValueProvider>> valprov;
  std::string toPointer;
  aggFunction aggfun;
  std::vector<std::unique_ptr<joda::query::IAggregator>> aggs;

  std::unique_ptr<joda::query::IValueProvider> groupedByValue = nullptr;
  std::string groupAs{};

};
}

#endif //JODA_AGG_STATE_H
