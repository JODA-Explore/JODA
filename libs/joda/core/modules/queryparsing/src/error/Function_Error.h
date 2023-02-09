
#ifndef JODA_FUNCTION_ERROR_H
#define JODA_FUNCTION_ERROR_H
#include "../grammar/Literals.h"
namespace joda::queryparsing::grammar {

template <>
const std::string query_control<padded_func>::error_message =
    "Expected a literal, pointer, or function call";

template <>
const std::string query_control<bracketEnd>::error_message =
    "Missing ')'";

template <>
const std::string query_control<bracketStart>::error_message =
    "Missing '('";

template <>
const std::string query_control<stringEnd>::error_message =
    "Missing '\"' at end of string";

template <>
const std::string query_control<pointerEnd>::error_message =
    "Missing '\'' at end of pointer";

}  // namespace joda::queryparsing::grammar
#endif  // JODA_FUNCTION_ERROR_H
