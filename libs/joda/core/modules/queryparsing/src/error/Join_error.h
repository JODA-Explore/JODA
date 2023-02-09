
#ifndef JODA_JOIN_ERROR_H
#define JODA_JOIN_ERROR_H
#include "../grammar/Join.h"
namespace joda::queryparsing::grammar {
/*
* ON
*/
template <>
const std::string query_control<join_on_start>::error_message =
    "Missing '(' after 'ON'";

template <>
const std::string query_control<join_on_end>::error_message =
    "Missing ')' after 'ON'";

template <>
const std::string query_control<join_on_first>::error_message =
    "Missing expression in 'JOIN <Outer> ON(<exp>)'";

template <>
const std::string query_control<join_on_second>::error_message =
    "Missing outer expression in 'JOIN <Outer> ON(<inner_exp>, <outer_exp>)'";

template <>
const std::string query_control<join_ident>::error_message =
    "Missing join partner after 'JOIN'";

template <>
const std::string query_control<join_outer>::error_message =
    "Expected '<collection name>' or '(<subquery>)' after `JOIN`";

template <>
const std::string query_control<join_on_kw>::error_message =
    "Missing 'ON' after 'JOIN <Outer>'";

template <>
const std::string query_control<join_subquery_command>::error_message =
    "Invalid subquery in JOIN expression";

template <>
const std::string query_control<join_subquery_start>::error_message =
    "Missing '(' after 'JOIN'";

template <>
const std::string query_control<join_subquery_end>::error_message =
    "Missing ')' after <subquery>. (Forgot ')' or sub-query has unallowed STORE command)";

}  // namespace joda::queryparsing::grammar
#endif  // JODA_JOIN_ERROR_H
