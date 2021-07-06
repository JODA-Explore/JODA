//
// Created by Nico on 21/02/2019.
//

#ifndef JODA_DELETE_H
#define JODA_DELETE_H

#include "../../../../../../../extern/PEGTL/include/tao/pegtl.hpp"

#include "Literals.h"
namespace joda::queryparsing::grammar {

/*
 * Forward Declaration Actions
 */
template <typename Rule>
struct deleteAction : tao::pegtl::nothing<Rule> {};

/*
 * Keywords
 */
#ifndef __CLION_IDE__  // Prevent lag from expanding all macros
struct deleteKW : TAOCPP_PEGTL_KEYWORD("DELETE") {};
#endif

struct deleteIdent : tao::pegtl::identifier {};

struct deleteCommand
    : tao::pegtl::opt<
          tao::pegtl::pad<deleteKW, tao::pegtl::space>,
          tao::pegtl::pad<tao::pegtl::must<deleteIdent>, tao::pegtl::space>> {};

}  // namespace joda::queryparsing::grammar

#endif  // JODA_DELETE_H
