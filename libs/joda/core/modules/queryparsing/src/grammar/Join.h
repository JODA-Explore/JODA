
#ifndef JODA_JOIN_H
#define JODA_JOIN_H

#include "../../../../../../../extern/PEGTL/include/tao/pegtl.hpp"
#include "helpers.h"

namespace joda::queryparsing::grammar {

/*
 * Forward Declaration Actions
 */
template <typename Rule>
struct joinAction : tao::pegtl::nothing<Rule> {};

struct queryCommandWithoutStore;
struct queryState;
template <typename Rule>
struct queryAction;



struct joinKW : TAO_PEGTL_KEYWORD("JOIN") {};



/*
* ON
*/
struct joinON_KW : TAO_PEGTL_KEYWORD("ON") {};
struct join_on_kw : padded<joinON_KW> {};

struct join_on_first : padded<predicate_expression> {};
struct join_on_second : padded<predicate_expression> {};

struct join_on_delimiter : padded<tao::pegtl::one<','>>{};

struct join_on_second_exp : padded<tao::pegtl::if_must<
  join_on_delimiter,
  join_on_second
>>{};

struct join_on_start : padded<tao::pegtl::one<'('>>{};
struct join_on_end : padded<tao::pegtl::one<')'>>{};

// ... (<first>[, second])
struct joinOn : padded<tao::pegtl::must<
join_on_start,
join_on_first,
tao::pegtl::opt<join_on_second_exp>,
join_on_end
>> {};

// ... ON <expr>
struct joinOn_exp :  tao::pegtl::if_must<
  join_on_kw,
  joinOn
> {};


/*
* WHERE
*/
struct joinWHERE_KW : TAO_PEGTL_KEYWORD("WHERE") {};
struct join_where_kw : padded<joinWHERE_KW> {};
struct join_where_predicate : padded<predicate_expression> {};
// .. WHERE <pred>
struct join_where_exp :  tao::pegtl::if_must<
  join_where_kw,
  join_where_predicate
> {};


/*
* Outer Join Partner
*/ 
struct join_ident : padded<loadIdent> {};
// - subquery
struct join_subquery_start : padded<tao::pegtl::one<'('>>{};
struct join_subquery_end : padded<tao::pegtl::one<')'>>{};
struct join_subquery_command : tao::pegtl::state<queryState,tao::pegtl::action<queryAction,queryCommandWithoutStore>>  {};
struct join_subquery : tao::pegtl::if_must<join_subquery_start,join_subquery_command,join_subquery_end> {};

struct join_outer: padded<tao::pegtl::sor<join_subquery,join_ident>> {};

// JOIN <...>
struct join_exp : tao::pegtl::must<
  join_outer,
  tao::pegtl::sor<joinOn_exp,join_where_exp> 
> {};

}  // namespace joda::queryparsing::grammar

#include "Grammar.h" // queryCommand, queryAction
#include "../states/Query_State.h" // queryState
#endif  // JODA_JOIN_H
