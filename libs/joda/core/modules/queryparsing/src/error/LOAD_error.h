//
// Created by Nico on 21/05/2019.
//

#ifndef JODA_LOAD_ERROR_H
#define JODA_LOAD_ERROR_H
namespace joda::queryparsing::grammar {

//Missing Load
template<> const std::string query_control<loadKW>::error_message = "Query must start with 'LOAD' statement.";

//Missing Load var
template<> const std::string query_control<loadIdent>::error_message = "Expected 'LOAD' variable. (LOAD X ...)";

//Missing LOAD X FROM FILE(S) "[...]"
template<> const std::string
    query_control<loadFilesliteral>::error_message = "Expected 'LOAD' file/directory specifier";
template<> const std::string
    query_control<loadFileLiteralStart>::error_message = "Expected '\"' after LOAD FROM FILE(S)";
template<> const std::string
    query_control<loadFileLiteralEnd>::error_message = "Expected '\"' after LOAD FROM FILE(S) \"[...]";
template<> const std::string query_control<loadFileName>::error_message = "Expected File/Directory name";
//
template<> const std::string query_control<loadCommand>::error_message = "Missing/Illegal 'LOAD' command";

template<> const std::string query_control<sampleNum>::error_message = "Expected sampling number in (0,1)";

template<> const std::string query_control<fromURLKW>::error_message = "FROM URL not supported.";

}
#endif //JODA_LOAD_ERROR_H
