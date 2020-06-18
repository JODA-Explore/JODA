//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_DELETE_STATE_H
#define JODA_DELETE_STATE_H
#include "Query_State.h"
namespace joda::queryparsing::grammar {
struct deleteState {
  template<typename Input>
  inline deleteState(const Input &in, queryState &qs) {
  }

  template<typename Input>
  inline void success(const Input &in, queryState &qs) {
    qs.q->setDelete(deleteVar);
  }

  std::string deleteVar;
};
}
#endif //JODA_DELETE_STATE_H
