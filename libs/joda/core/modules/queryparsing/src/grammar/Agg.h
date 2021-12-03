//
// Created by Nico on 21/02/2019.
//

#ifndef JODA_AGG_H
#define JODA_AGG_H
#include "../../../../../../../extern/PEGTL/include/tao/pegtl.hpp"
#include "Literals.h"
namespace joda::queryparsing::grammar {

/*
 * Forward Declaration Actions
 */
template <typename Rule>
struct aggExpAction : tao::pegtl::nothing<Rule> {};

/*
 * Keywords
 */
#ifndef __CLION_IDE__  // Prevent lag from expanding all macros
struct aggKW : TAOCPP_PEGTL_KEYWORD("AGG") {};

struct aggKW_MIN : TAOCPP_PEGTL_KEYWORD("MIN") {};
struct aggKW_MAX : TAOCPP_PEGTL_KEYWORD("MAX") {};
struct aggKW_ATTSTAT : TAOCPP_PEGTL_KEYWORD("ATTSTAT") {};
struct aggKW_AVG : TAOCPP_PEGTL_KEYWORD("AVG") {};
struct aggKW_COUNT : TAOCPP_PEGTL_KEYWORD("COUNT") {};
struct aggKW_SUM : TAOCPP_PEGTL_KEYWORD("SUM") {};
struct aggKW_DISTINCT : TAOCPP_PEGTL_KEYWORD("DISTINCT") {};
struct aggKW_COLLECT : TAOCPP_PEGTL_KEYWORD("COLLECT") {};
struct aggKW_HISTOGRAM : TAOCPP_PEGTL_KEYWORD("HISTOGRAM") {};

struct aggKW_GROUP : TAOCPP_PEGTL_KEYWORD("GROUP") {};
struct aggKW_BY : TAOCPP_PEGTL_KEYWORD("BY") {};
struct aggKW_AS : TAOCPP_PEGTL_KEYWORD("AS") {};
#endif

// Agg commands
struct aggKeywords
    : tao::pegtl::sor<aggKW_ATTSTAT, aggKW_AVG, aggKW_COUNT, aggKW_SUM,
                      aggKW_DISTINCT, aggKW_COLLECT, aggKW_MIN, aggKW_MAX, aggKW_HISTOGRAM> {};

/*
 * Aggregate Expresstion
 */
struct aggFromPointer : pointer {};
struct aggToPointer : pointer {};
struct aggFrom
    : tao::pegtl::seq<
          aggKeywords, tao::pegtl::pad<tao::pegtl::one<'('>, tao::pegtl::space>,
          tao::pegtl::opt<
              tao::pegtl::list<functionstateaction, tao::pegtl::one<','>,
                               tao::pegtl::space>>,
              tao::pegtl::pad<tao::pegtl::one<')'>, tao::pegtl::space>> {};

/*
 * Aggregation Group
 */
struct aggByExpr
    : tao::pegtl::must<
          tao::pegtl::pad<functionstateaction, tao::pegtl::space>> {};
struct aggAsIdent : tao::pegtl::identifier {};
struct aggGrouped
    : tao::pegtl::seq<aggKW_GROUP, tao::pegtl::pad<aggFrom, tao::pegtl::space>,
                      tao::pegtl::opt<tao::pegtl::seq<
                          tao::pegtl::pad<aggKW_AS, tao::pegtl::space>,
                          tao::pegtl::pad<aggAsIdent, tao::pegtl::space>>>,
                      tao::pegtl::pad<aggKW_BY, tao::pegtl::space>,
                      tao::pegtl::pad<aggByExpr, tao::pegtl::space>> {};

struct aggSingleExp
    : tao::pegtl::seq<
          tao::pegtl::must<tao::pegtl::one<'('>>,
          tao::pegtl::pad<tao::pegtl::must<aggToPointer>, tao::pegtl::space>,
          tao::pegtl::must<
              tao::pegtl::pad<tao::pegtl::one<':'>, tao::pegtl::space>>,
          tao::pegtl::pad<tao::pegtl::sor<aggGrouped, aggFrom>,
                          tao::pegtl::space>,
          tao::pegtl::pad<tao::pegtl::must<tao::pegtl::one<')'>>,
                          tao::pegtl::space>> {};
struct aggExp : tao::pegtl::action<
                    aggExpAction,
                    tao::pegtl::list_must<aggSingleExp, tao::pegtl::one<','>,
                                          tao::pegtl::space>> {};

}  // namespace joda::queryparsing::grammar

#endif  // JODA_AGG_H
