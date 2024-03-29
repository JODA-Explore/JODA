//
// Created by Nico on 21/02/2019.
//

#ifndef JODA_AS_H
#define JODA_AS_H

#include "../../../../../../../extern/PEGTL/include/tao/pegtl.hpp"
#include "Literals.h"

namespace joda::queryparsing::grammar {

/*
 * Forward Declaration Actions
 */
template <typename Rule>
struct asExpAction : tao::pegtl::nothing<Rule> {};
/*
 * Keywords
 */
#ifndef __CLION_IDE__  // Prevent lag from expanding all macros
struct asKW : TAO_PEGTL_KEYWORD("AS") {};

struct projectKW_ARRFLAT : TAO_PEGTL_KEYWORD("FLATTEN") {};
struct projectKW_ALL : tao::pegtl::one<'*'> {};
#endif

struct setProjectionKW : tao::pegtl::sor<projectKW_ARRFLAT> {};
struct projectFromPointer : pointer {};
struct setProjectParam
    : tao::pegtl::sor<projectFromPointer, functionstateaction> {};

struct setProjectionExp
    : tao::pegtl::if_must<setProjectionKW,
                          tao::pegtl::seq<tao::pegtl::one<'('>, setProjectParam,
                                          tao::pegtl::one<')'>>> {};
struct projectFrom : tao::pegtl::sor<setProjectionExp, functionstateaction,
                                     projectFromPointer, tao::pegtl::success> {
};

struct projectToPointer : pointer {};
struct projectTupleExp
    : tao::pegtl::seq<
          tao::pegtl::must<tao::pegtl::one<'('>>,
          tao::pegtl::pad<tao::pegtl::must<projectToPointer>,
                          tao::pegtl::space>,
          tao::pegtl::pad<tao::pegtl::must<tao::pegtl::one<':'>>,
                          tao::pegtl::space>,
          tao::pegtl::pad<tao::pegtl::must<projectFrom>, tao::pegtl::space>,
          tao::pegtl::must<
              tao::pegtl::pad<tao::pegtl::one<')'>, tao::pegtl::space>>> {};

struct projectSingleExp : tao::pegtl::sor<projectKW_ALL, projectTupleExp> {};

struct projectExp
    : tao::pegtl::list_must<projectSingleExp, tao::pegtl::one<','>,
                            tao::pegtl::space> {};

}  // namespace joda::queryparsing::grammar

#endif  // JODA_AS_H
