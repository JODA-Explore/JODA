

#ifndef JODA_JOIN_STATE_H
#define JODA_JOIN_STATE_H

#include <joda/query/values/IValueProvider.h>
#include <joda/join/relation/EqualityJoinExecutor.h>
#include <joda/join/relation/ThetaJoinExecutor.h>

#include "Query_State.h"

namespace joda::queryparsing::grammar {

enum class JoinType { NoJoin, EqualityJoin, ThetaJoin };

struct joinState {
  template <typename Input>
  inline joinState(const Input &in, queryState &qs) {}

  template <typename Input>
  inline void success(const Input &in, queryState &qs) {
    if (type == JoinType::NoJoin) return;
    try{
    if(type == JoinType::EqualityJoin){
      DCHECK(lhs != nullptr);
      if(rhs == nullptr){
        rhs = lhs->duplicate();
      }
      qs.q->setJoinExecutor(std::make_shared<joda::join::EqualityJoinExecutor>(std::move(lhs), std::move(rhs)));
      qs.q->setJoinPartner(outerIdent);
      qs.q->setSubQuery(std::move(subquery));
    }else if(type == JoinType::ThetaJoin){
      DCHECK(lhs != nullptr);
      qs.q->setJoinExecutor(std::make_shared<joda::join::ThetaJoinExecutor>(std::move(lhs)));
      qs.q->setJoinPartner(outerIdent);
      qs.q->setSubQuery(std::move(subquery));
    }
    } catch (const std::exception &e) {
      // Expect all IValue/Join Exceptions
      throw tao::pegtl::parse_error(e.what(), in);
    }
  }

  void addFunc(std::unique_ptr<joda::query::IValueProvider>&& func){
    if(lhs == nullptr){
      lhs = std::move(func);
    }else if(rhs == nullptr){
      rhs = std::move(func);
    }
  }

  JoinType type = JoinType::NoJoin;
  std::string outerIdent;
  std::unique_ptr<joda::query::IValueProvider> lhs;
  std::unique_ptr<joda::query::IValueProvider> rhs;
  std::shared_ptr<joda::query::Query> subquery;
};

}  // namespace joda::queryparsing::grammar
#endif  // JODA_JOIN_STATE_H
