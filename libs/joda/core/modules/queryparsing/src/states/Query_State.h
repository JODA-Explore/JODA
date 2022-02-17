//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_QUERY_STATE_H
#define JODA_QUERY_STATE_H
#include <joda/query/predicate/Predicate.h>

#include <stack>
namespace joda::queryparsing::grammar {

struct queriesState {
  inline queriesState() {}

  template <typename Input>
  inline void success(const Input &in){};

  std::vector<std::shared_ptr<joda::query::Query>> q{};
};

struct queryState {
  inline queryState() { q = std::make_shared<joda::query::Query>(); }

  template <typename Input>
  inline queryState(const Input &in, queriesState &qs) {q = std::make_shared<joda::query::Query>();}

  template <typename Input>
  inline void success(const Input &in){};

  template <typename Input>
  inline void success(const Input &in, queriesState &state) {
    state.q.emplace_back(std::move(q));
  };

  std::shared_ptr<joda::query::Query> q;
};

enum Comparison { NONE, GT, GTE, LT, LTE, EQU, NEQU };
enum stackMod { BASE, AND, OR };

typedef std::stack<
    std::pair<stackMod, std::vector<std::unique_ptr<joda::query::Predicate>>>>
    predStack;
}  // namespace joda::queryparsing::grammar
#endif  // JODA_QUERY_STATE_H
