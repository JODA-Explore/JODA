#ifndef JODA_GRAMMAR_HELPERS_H
#define JODA_GRAMMAR_HELPERS_H

#include <tao/pegtl.hpp>

namespace joda::queryparsing::grammar {

template <typename Rule>
struct padded : tao::pegtl::pad<Rule, tao::pegtl::space> {};

template <typename Rule>
struct padded_must : tao::pegtl::pad<tao::pegtl::must<Rule>, tao::pegtl::space> {};

}

#endif // JODA_GRAMMAR_HELPERS_H