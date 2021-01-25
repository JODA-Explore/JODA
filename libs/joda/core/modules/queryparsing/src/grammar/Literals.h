//
// Created by Nico on 21/02/2019.
//

#ifndef JODA_LITERALS_H
#define JODA_LITERALS_H

#include "../../../../../../../extern/PEGTL/include/tao/pegtl.hpp"

namespace joda::queryparsing::grammar {

/*
 * JSON Pointer
 */
struct escapedToken : tao::pegtl::seq<tao::pegtl::one<'~'>, tao::pegtl::sor<tao::pegtl::one<'0'>, tao::pegtl::one<'1'>>> {};
struct unescapedToken : tao::pegtl::utf8::not_one<'\x2F', '\x7E', '\''> {};
struct referenceToken : tao::pegtl::star<tao::pegtl::sor<unescapedToken, escapedToken>> {};
struct pointer : tao::pegtl::seq<tao::pegtl::one<'\''>, tao::pegtl::star<tao::pegtl::seq<tao::pegtl::one<'/'>, referenceToken>>, tao::pegtl::one<'\''>> {};

//Functions
struct funcPtr : pointer {};

/*
 * Number
 */
struct floatNumber : tao::pegtl::seq<
    tao::pegtl::opt<tao::pegtl::one<'+', '-'> >,
    tao::pegtl::plus<tao::pegtl::digit>,
    tao::pegtl::one<'.'>,
    tao::pegtl::plus<tao::pegtl::digit>
> {
};

struct intNumber : tao::pegtl::seq<
    tao::pegtl::opt<tao::pegtl::one<'+', '-'> >,
    tao::pegtl::plus<tao::pegtl::digit>
> {
};

struct numberAtom : tao::pegtl::sor<floatNumber, intNumber> {};

/*
 * String
 */
struct escapedChar : tao::pegtl::seq<tao::pegtl::utf8::one<'\\'>, tao::pegtl::sor<tao::pegtl::utf8::one<'"'>, tao::pegtl::utf8::one<'\\'>>> {};
struct unescapedChar : tao::pegtl::utf8::not_one<'\\', '"'> {};
struct stringAtom : tao::pegtl::seq<tao::pegtl::one<'"'>, tao::pegtl::star<tao::pegtl::sor<escapedChar, unescapedChar>>, tao::pegtl::one<'"'>> {};

/*
 * Bool
 */

#ifndef __CLION_IDE__ //Prevent lag from expanding all macros
struct boolAtom : tao::pegtl::sor<TAOCPP_PEGTL_KEYWORD("true"), TAOCPP_PEGTL_KEYWORD("false")> {};
#endif

/*
 * Null
 */

#ifndef __CLION_IDE__ //Prevent lag from expanding all macros
struct nullAtom : tao::pegtl::sor<TAOCPP_PEGTL_KEYWORD("null"), TAOCPP_PEGTL_KEYWORD("NULL")> {};
#endif

/*
 * Expression
 */

struct funcBracketOpen : tao::pegtl::one<'('> {};
struct funcBracketClose : tao::pegtl::one<')'> {};


struct gt : tao::pegtl::seq<tao::pegtl::one<'>'>, tao::pegtl::opt<tao::pegtl::one<'='>>> {};
struct lt : tao::pegtl::seq<tao::pegtl::one<'<'>, tao::pegtl::opt<tao::pegtl::one<'='>>> {};
struct equal : tao::pegtl::seq<tao::pegtl::sor<tao::pegtl::one<'!'>, tao::pegtl::one<'='>>, tao::pegtl::one<'='>> {};
struct compare : tao::pegtl::sor<gt, lt,equal> {};
struct listContent : tao::pegtl::sor<numberAtom, stringAtom, boolAtom,nullAtom> {};
struct beginList : tao::pegtl::one<'['> {};
//struct numList : tao::pegtl::seq<beginList, tao::pegtl::opt<tao::pegtl::list<tao::pegtl::listContent, tao::pegtl::one<','>, tao::pegtl::space>>, tao::pegtl::one<']'>> {};
struct varExp : tao::pegtl::sor<numberAtom, stringAtom, boolAtom,nullAtom, pointer> {};
struct functionstateaction;
struct funcParamRule : tao::pegtl::sor<numberAtom, stringAtom, pointer, boolAtom,nullAtom, /*numList,*/ functionstateaction> {};
struct realFunc : tao::pegtl::seq<functionKWs,
                                  tao::pegtl::must<funcBracketOpen>,
                                  tao::pegtl::opt<tao::pegtl::list<funcParamRule,
                                                                   tao::pegtl::one<','>,
                                                                   tao::pegtl::space>>,
                                  tao::pegtl::must<funcBracketClose>> {
};
struct atomFunc : varExp{};
struct ptrFunc : tao::pegtl::sor<varExp,realFunc>{};
struct functionstateaction : tao::pegtl::state<functionState, tao::pegtl::action<functionAction, ptrFunc>> {};


struct compareExp : tao::pegtl::seq<tao::pegtl::must<tao::pegtl::pad<functionstateaction, tao::pegtl::space>>, tao::pegtl::opt<tao::pegtl::pad<compare, tao::pegtl::space>, tao::pegtl::pad<tao::pegtl::must<functionstateaction>, tao::pegtl::space>>> {};


struct orExp;
struct negate : tao::pegtl::one<'!'>{};
struct bracketStart : tao::pegtl::pad<tao::pegtl::one<'('>, tao::pegtl::space> {};
struct bracketEnd : tao::pegtl::pad<tao::pegtl::one<')'>, tao::pegtl::space> {};
struct atomExp : tao::pegtl::sor<tao::pegtl::seq<bracketStart, tao::pegtl::pad<orExp, tao::pegtl::space>, bracketEnd>,compareExp> {};
struct unaryExp : tao::pegtl::seq<tao::pegtl::pad_opt<negate,tao::pegtl::space>, atomExp> {};
struct andStart : tao::pegtl::star<tao::pegtl::space> {};
struct andSymbol : tao::pegtl::seq<tao::pegtl::one<'&'>,tao::pegtl::one<'&'>>{};
struct andExp : tao::pegtl::seq<andStart, tao::pegtl::list_must<unaryExp, andSymbol, tao::pegtl::space> > {};
struct orSymbol : tao::pegtl::seq<tao::pegtl::one<'|'>, tao::pegtl::one<'|'>> {};
struct orStart : tao::pegtl::star<tao::pegtl::space> {};
struct orExp : tao::pegtl::seq<orStart, tao::pegtl::list_must<andExp, orSymbol, tao::pegtl::space>> {};
struct expStart : tao::pegtl::star<tao::pegtl::space> {};
struct qexp : tao::pegtl::seq<expStart, orExp> {};

/*
 * Group Expression
 */
struct groupPointer : pointer {};
struct groupFunction : functionstateaction {};
struct groupIdent : tao::pegtl::sor<groupPointer,groupFunction>{};

}

#endif //JODA_LITERALS_H
