//
// Created by Nico on 21/02/2019.
//

#ifndef JODA_LITERALS_H
#define JODA_LITERALS_H

#include "../../../../../../../extern/PEGTL/include/tao/pegtl.hpp"
#include "../FunctionWrapper.h"
#include "helpers.h"

namespace joda::queryparsing::grammar {

/*
 * JSON Pointer
 */
struct escapedToken
    : tao::pegtl::seq<
          tao::pegtl::one<'~'>,
          tao::pegtl::sor<tao::pegtl::one<'0'>, tao::pegtl::one<'1'>>> {};
struct unescapedToken : tao::pegtl::utf8::not_one<'\x2F', '\x7E', '\''> {};
struct referenceToken
    : tao::pegtl::star<tao::pegtl::sor<unescapedToken, escapedToken>> {};

struct pointerStart : tao::pegtl::one<'\''> {};
struct pointerEnd : tao::pegtl::one<'\''> {};

struct pointer_raw
    : tao::pegtl::if_must<pointerStart,
                          tao::pegtl::star<tao::pegtl::seq<tao::pegtl::one<'/'>,
                                                           referenceToken>>,
                          pointerEnd> {};

struct unprefixed_pointer : pointer_raw {};
struct prefixed_pointer : tao::pegtl::seq<tao::pegtl::one<'$'>, tao::pegtl::opt<pointer_raw>> {};
struct pointer : tao::pegtl::sor<prefixed_pointer, unprefixed_pointer> {};


/*
 * Number
 */
struct floatNumber
    : tao::pegtl::seq<tao::pegtl::opt<tao::pegtl::one<'+', '-'>>,
                      tao::pegtl::plus<tao::pegtl::digit>, tao::pegtl::one<'.'>,
                      tao::pegtl::plus<tao::pegtl::digit>> {};

struct unsignedIntNumber : tao::pegtl::plus<tao::pegtl::digit> {};
struct intNumber : tao::pegtl::seq<tao::pegtl::opt<tao::pegtl::one<'+', '-'>>,
                                   tao::pegtl::plus<tao::pegtl::digit>> {};

struct numberAtom : tao::pegtl::sor<floatNumber, intNumber> {};

/*
 * String
 */
struct escapedChar
    : tao::pegtl::seq<tao::pegtl::utf8::one<'\\'>,
                      tao::pegtl::sor<tao::pegtl::utf8::one<'"'>,
                                      tao::pegtl::utf8::one<'\\'>>> {};
struct unescapedChar : tao::pegtl::utf8::not_one<'\\', '"'> {};

struct stringStart : tao::pegtl::one<'"'> {};
struct stringEnd : tao::pegtl::one<'"'> {};

struct stringAtom
    : tao::pegtl::if_must<
          stringStart,
          tao::pegtl::star<tao::pegtl::sor<escapedChar, unescapedChar>>,
          stringEnd> {};

/*
 * Bool
 */

#ifndef __CLION_IDE__  // Prevent lag from expanding all macros
struct boolAtom
    : tao::pegtl::sor<TAO_PEGTL_KEYWORD("true"), TAO_PEGTL_KEYWORD("false")> {};
#endif

/*
 * Null
 */

#ifndef __CLION_IDE__  // Prevent lag from expanding all macros
struct nullAtom
    : tao::pegtl::sor<TAO_PEGTL_KEYWORD("null"), TAO_PEGTL_KEYWORD("NULL")> {};
#endif

/*
 * Expression
 */

struct or_expression;  // Forward declaration

struct funcBracketOpen : tao::pegtl::one<'('> {};
struct funcBracketClose : padded<tao::pegtl::one<')'>> {};

struct and_equal : tao::pegtl::one<'='> {};

struct gt : tao::pegtl::seq<tao::pegtl::one<'>'>, tao::pegtl::opt<and_equal>> {
};
struct lt : tao::pegtl::seq<tao::pegtl::one<'<'>, tao::pegtl::opt<and_equal>> {
};

struct unequal : tao::pegtl::one<'!'> {};
struct equal : tao::pegtl::one<'='> {};

struct equality
    : tao::pegtl::seq<tao::pegtl::sor<unequal, equal>, tao::pegtl::one<'='>> {};
struct compare : tao::pegtl::sor<gt, lt, equality> {};
struct listContent
    : tao::pegtl::sor<numberAtom, stringAtom, boolAtom, nullAtom> {};
struct beginList : tao::pegtl::one<'['> {};
// struct numList : tao::pegtl::seq<beginList,
// tao::pegtl::opt<tao::pegtl::list<tao::pegtl::listContent,
// tao::pegtl::one<','>, tao::pegtl::space>>, tao::pegtl::one<']'>> {};
struct varExp
    : tao::pegtl::sor<numberAtom, stringAtom, boolAtom, nullAtom, pointer> {};
struct functionstateaction;
struct funcParamRule : tao::pegtl::sor<or_expression, numberAtom, stringAtom,
                                       pointer, boolAtom, nullAtom
                                       /*numList,*/> {};
struct realFunc
    : tao::pegtl::seq<
          functionKWs, tao::pegtl::must<funcBracketOpen>,
          tao::pegtl::opt<tao::pegtl::list<funcParamRule, tao::pegtl::one<','>,
                                           tao::pegtl::space>>,
          tao::pegtl::must<funcBracketClose>> {};
struct atomFunc : varExp {};
struct ptrFunc : tao::pegtl::sor<varExp, realFunc> {};
struct functionstateaction
    : tao::pegtl::state<functionState,
                        tao::pegtl::action<functionAction, ptrFunc>> {};

struct negate : tao::pegtl::one<'!'> {};
struct bracketStart : tao::pegtl::pad<tao::pegtl::one<'('>, tao::pegtl::space> {
};
struct bracketEnd : tao::pegtl::pad<tao::pegtl::one<')'>, tao::pegtl::space> {};

struct andSymbol : tao::pegtl::seq<tao::pegtl::one<'&'>, tao::pegtl::one<'&'>> {
};
struct orSymbol : tao::pegtl::seq<tao::pegtl::one<'|'>, tao::pegtl::one<'|'>> {
};

// New

struct padded_func : padded<functionstateaction> {};

struct padded_compare : padded<compare> {};

struct compare_expression
    : tao::pegtl::state<CompareState,
                        tao::pegtl::seq<padded_func,
                                        tao::pegtl::opt<tao::pegtl::if_must<
                                            padded_compare, padded_func>>>> {};

struct recurse_expression
    : tao::pegtl::sor<
          tao::pegtl::if_must<bracketStart, padded<or_expression>, bracketEnd>,
          compare_expression> {};

struct boolean_expression
    : tao::pegtl::state<
          BoolState,
          tao::pegtl::seq<tao::pegtl::pad_opt<negate, tao::pegtl::space>,
                          recurse_expression>> {};

struct and_expression
    : tao::pegtl::state<AndState,
                        tao::pegtl::list_must<boolean_expression, andSymbol,
                                              tao::pegtl::space>> {};

struct or_expression
    : tao::pegtl::state<OrState, tao::pegtl::list_must<and_expression, orSymbol,
                                                       tao::pegtl::space>> {};

struct predicate_expression
    : tao::pegtl::action<functionAction,
                         tao::pegtl::state<ValueState, or_expression>> {};

/*
 * Group Expression
 */
struct groupPointer : pointer {};
struct groupFunction : functionstateaction {};
struct groupIdent : tao::pegtl::sor<groupPointer, groupFunction> {};

}  // namespace joda::queryparsing::grammar

#endif  // JODA_LITERALS_H
