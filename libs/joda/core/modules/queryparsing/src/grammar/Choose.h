//
// Created by Nico on 21/02/2019.
//

#ifndef JODA_CHOOSE_H
#define JODA_CHOOSE_H

#include "../../../../../../../extern/PEGTL/include/tao/pegtl.hpp"

namespace joda::queryparsing::grammar {

/*
 * Forward Declaration Actions
 */
template <typename Rule>
struct chooseExpAction : tao::pegtl::nothing<Rule> {};

/*
 * Keywords
 */
#ifndef __CLION_IDE__  // Prevent lag from expanding all macros
struct chooseKW : TAO_PEGTL_KEYWORD("CHOOSE") {};
#endif

}  // namespace joda::queryparsing::grammar

#endif  // JODA_CHOOSE_H
