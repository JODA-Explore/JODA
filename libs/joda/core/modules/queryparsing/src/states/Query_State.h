//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_QUERY_STATE_H
#define JODA_QUERY_STATE_H
#include <stack>
namespace joda::queryparsing::grammar {

struct queriesState {
  inline queriesState() {}

  template <typename Input>
  inline void success(const Input &in){};

  std::vector<std::shared_ptr<joda::query::Query>> q{};
};

struct joinState;  // Forward declare JoinState for subquery expressions

struct queryState {
  inline queryState() { q = std::make_shared<joda::query::Query>(); }

  // JOIN Subquery
  template <typename Input>
  inline void success(const Input &in,
                      joinState &qs);  // Forward declare success function

  template <typename Input>
  inline void success(const Input &in){};

  template <typename Input>
  inline void success(const Input &in, queriesState &state) {
    state.q.emplace_back(std::move(q));
  };

  std::shared_ptr<joda::query::Query> q;
};

}  // namespace joda::queryparsing::grammar

#include "Join_State.h"  // Import joinstate (cyclic dependency)

namespace joda::queryparsing::grammar {
// Actually define function
template <typename Input>
inline void queryState::success(const Input &in, joinState &qs) {
  qs.subquery = std::move(q);
}
}  // namespace joda::queryparsing::grammar

#endif  // JODA_QUERY_STATE_H
