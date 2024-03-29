//
// Created by Nico Schäfer on 11/23/17.
//

#ifndef JODA_CONTROL_H
#define JODA_CONTROL_H
#include <tao/pegtl/normal.hpp>
namespace joda::queryparsing::grammar {
template <typename Rule>
struct query_control : tao::pegtl::normal<Rule> {
  static const std::string error_message;

  template <typename Input, typename... States>
  inline static void raise(const Input &in, States &&...) {
    throw tao::pegtl::parse_error(error_message, in);
  }
};
}  // namespace joda::queryparsing::grammar
#include "error/Load_error.h"
#include "error/Join_error.h"
#include "error/Function_Error.h"
#include "grammar/Grammar.h"

namespace joda::queryparsing::grammar {
/*
 * Errors
 */

// Store
template <>
const std::string query_control<storeFileliteral>::error_message =
    "Missing 'STORE' file specifier";
template <>
const std::string query_control<tao::pegtl::sor<
    storeAsFileCommand, tao::pegtl::pad<storeIdent, tao::pegtl::space>>>::
    error_message = "'STORE' command is missing store specifier";
template <>
const std::string query_control<inStreamKW>::error_message =
    "IN STREAM not available without output stream.";

// Unknown Aggregation KW
template <>
const std::string query_control<aggKeywords>::error_message =
    "Unknown aggregation function";
template <>
const std::string query_control<aggFromPointer>::error_message =
    "Illegal 'From-Pointer' in 'AGG' command";
template <>
const std::string query_control<aggToPointer>::error_message =
    "Illegal 'To-Pointer' in 'AGG' command";
template <>
const std::string query_control<aggExp>::error_message =
    "Error in 'AGG' command";

// Unknown Projection FROM
template <>
const std::string query_control<projectFrom>::error_message =
    "Unknown/Illegal 'From-Expression' in 'AS' command";
template <>
const std::string query_control<projectToPointer>::error_message =
    "Illegal 'To-Pointer' in 'AS' command";
template <>
const std::string query_control<projectExp>::error_message =
    "Error in 'AS' command";
template <>
const std::string query_control<projectSingleExp>::error_message =
    "Expected '*' or '(<Ptr>:<Source>)'";

// Choose
template <>
const std::string query_control<predicate_expression>::error_message =
    "Error in 'CHOOSE' expression. Does the function return a boolean? If not try using the TRUTHY function.";
template <>
const std::string query_control<functionstateaction>::error_message =
    "Atomic value, pointer or function expected.";
template <>
const std::string query_control<ptrFunc>::error_message =
    "Atomic value or function expected.";

/*
 * Generic erros
 */

// Brackets expected
template <>
const std::string query_control<
    tao::pegtl::pad<tao::pegtl::one<'('>, tao::pegtl::space>>::error_message =
    "Expected '('";
template <>
const std::string query_control<funcBracketOpen>::error_message =
    "Expected '('";
template <>
const std::string query_control<tao::pegtl::one<'('>>::error_message =
    "Expected '('";
template <>
const std::string query_control<
    tao::pegtl::pad<tao::pegtl::one<')'>, tao::pegtl::space>>::error_message =
    "Expected ')'";
template <>
const std::string query_control<tao::pegtl::one<')'>>::error_message =
    "Expected ')'";
template <>
const std::string query_control<funcBracketClose>::error_message =
    "Expected ',' or  ')'";
template <>
const std::string query_control<tao::pegtl::one<'"'>>::error_message =
    "Expected '\"'";

//: expected
template <>
const std::string query_control<
    tao::pegtl::pad<tao::pegtl::one<':'>, tao::pegtl::space>>::error_message =
    "Expected ':'";
template <>
const std::string query_control<tao::pegtl::one<':'>>::error_message =
    "Expected ':'";

// Wrong command/eof
template <>
const std::string query_control<queryCommand>::error_message =
    "Expected JOIN/CHOOSE/AS/AGG/STORE/EOF";

/*
 * Default
 */
template <typename T>
const std::string query_control<T>::error_message =
    "Parse error matching " + std::string(tao::pegtl::demangle<T>());

}  // namespace joda::queryparsing::grammar
#endif  // JODA_CONTROL_H
