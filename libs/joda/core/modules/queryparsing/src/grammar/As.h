//
// Created by Nico on 21/02/2019.
//

#ifndef JODA_AS_H
#define JODA_AS_H

#include "../../../../../../../extern/PEGTL/include/tao/pegtl.hpp"

namespace joda::queryparsing::grammar {

/*
 * Forward Declaration Actions
 */
template<typename Rule>
struct asExpAction
    : tao::pegtl::nothing<Rule> {
};
/*
 * Keywords
 */
#ifndef __CLION_IDE__ //Prevent lag from expanding all macros
struct asKW : TAOCPP_PEGTL_KEYWORD("AS") {};

struct projectKW_ARRFLAT : TAOCPP_PEGTL_KEYWORD("FLATTEN") {};
#endif

#include "Literals.h"


struct setProjectionKW : tao::pegtl::sor<projectKW_ARRFLAT> {};
struct projectFromPointer : pointer {};
struct setProjectParam : tao::pegtl::sor<projectFromPointer, functionstateaction> {};

struct setProjectionExp : tao::pegtl::if_must<setProjectionKW, tao::pegtl::seq<
    tao::pegtl::one<'('>,
    setProjectParam,
    tao::pegtl::one<')'>>> {
};
struct projectFrom : tao::pegtl::sor<setProjectionExp,
                                     functionstateaction,
                                     projectFromPointer
> {
};

struct projectToPointer : pointer {};
struct projectSingleExp : tao::pegtl::seq<
    tao::pegtl::must<tao::pegtl::one<'('>>,
    tao::pegtl::pad<tao::pegtl::must<projectToPointer>, tao::pegtl::space>,
    tao::pegtl::pad<tao::pegtl::must<tao::pegtl::one<':'>>, tao::pegtl::space>,
    tao::pegtl::pad<tao::pegtl::must<projectFrom>, tao::pegtl::space>,
    tao::pegtl::must<tao::pegtl::pad<tao::pegtl::one<')'>, tao::pegtl::space>>
> {
};
struct projectExp : tao::pegtl::list_must<projectSingleExp, tao::pegtl::one<','>, tao::pegtl::space> {};

}

#endif //JODA_AS_H
