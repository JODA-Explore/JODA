//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_QUERY_STATE_H
#define JODA_QUERY_STATE_H
#include <stack>
#include <joda/query/predicate/Predicate.h>
namespace joda::queryparsing::grammar {
struct queryState {
  inline queryState() {
    q = std::make_shared<joda::query::Query>();
  }

  std::shared_ptr<joda::query::Query> q;
};

enum Comparison { NONE, GT, GTE, LT, LTE, EQU, NEQU };
enum stackMod { BASE, AND, OR };

typedef std::stack<std::pair<stackMod, std::vector<std::unique_ptr<joda::query::Predicate>>>> predStack;
}
#endif //JODA_QUERY_STATE_H
