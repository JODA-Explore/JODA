//
// Created by Nico Schäfers on 09.November.17.
//

#ifndef PARSER_GRAMMAR_H_H
#define PARSER_GRAMMAR_H_H

#include <joda/query/predicate/AndPredicate.h>
#include <joda/query/predicate/ComparePredicate.h>
#include <joda/query/predicate/EqualizePredicate.h>
#include <joda/query/predicate/NegatePredicate.h>
#include <joda/query/predicate/OrPredicate.h>
#include <joda/query/project/PointerCopyProject.h>
#include <cctype>
#include <climits>
#include <functional>
#include <stack>
#include <string>

#include "../../../../../../../extern/PEGTL/include/tao/pegtl.hpp"

namespace joda::queryparsing::grammar {

/*
 * Forward Declaration Actions
 */
template <typename Rule>
struct queryAction : tao::pegtl::nothing<Rule> {};

template <typename Rule>
struct queriesAction : tao::pegtl::nothing<Rule> {};


}  // namespace joda::queryparsing::grammar

#include "Literals.h"

#include "Agg.h"
#include "As.h"
#include "Choose.h"
#include "Delete.h"
#include "Load.h"
#include "Store.h"

namespace joda::queryparsing::grammar {
/*
 * Query
 */

struct queryCommand
    : tao::pegtl::seq<
          tao::pegtl::state<
              loadState,
              tao::pegtl::action<loadAction, tao::pegtl::must<loadCommand>>>,
          tao::pegtl::state<
              chooseState,
              tao::pegtl::action<
                  chooseExpAction,
                  tao::pegtl::opt<tao::pegtl::pad<chooseKW, tao::pegtl::space>,
                                  tao::pegtl::pad<tao::pegtl::must<qexp>,
                                                  tao::pegtl::space>>>>,
          tao::pegtl::state<
              asState,
              tao::pegtl::action<
                  asExpAction,
                  tao::pegtl::opt<tao::pegtl::pad<asKW, tao::pegtl::space>,
                                  tao::pegtl::pad<tao::pegtl::must<projectExp>,
                                                  tao::pegtl::space>>>>,
          tao::pegtl::state<
              aggState,
              tao::pegtl::action<
                  aggExpAction,
                  tao::pegtl::opt<tao::pegtl::pad<aggKW, tao::pegtl::space>,
                                  tao::pegtl::pad<tao::pegtl::must<aggExp>,
                                                  tao::pegtl::space>>>>,
          tao::pegtl::state<
              storeState,
              tao::pegtl::action<storeAction, tao::pegtl::opt<storeCommand>>>,
          tao::pegtl::state<
              deleteState,
              tao::pegtl::action<deleteAction, tao::pegtl::opt<deleteCommand>>>,
          tao::pegtl::eof> {};

struct query : tao::pegtl::must<queryCommand> {};

struct single_query : tao::pegtl::state<
              queryState,
              tao::pegtl::action<
                  queryAction, queryCommand>> {};

struct query_separator : tao::pegtl::one<';'> {};

struct query_list : tao::pegtl::list_tail<single_query, query_separator,
                                                       tao::pegtl::space> {};

struct queries : tao::pegtl::must<query_list,tao::pegtl::star<tao::pegtl::space>,
          tao::pegtl::eof> {};


}  // namespace joda::queryparsing::grammar
#endif  // PARSER_GRAMMAR_H_H
