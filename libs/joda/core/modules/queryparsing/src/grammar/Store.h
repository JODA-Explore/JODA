//
// Created by Nico on 21/02/2019.
//

#ifndef JODA_STORESTATEMENT_H
#define JODA_STORESTATEMENT_H
#include "../../../../../../../extern/PEGTL/include/tao/pegtl.hpp"

namespace joda::queryparsing::grammar {
/*
 * Forward Declaration Actions
 */
template <typename Rule>
struct storeAction : tao::pegtl::nothing<Rule> {};

/*
 * Keywords
 */

#ifndef __CLION_IDE__  // Prevent lag from expanding all macros
struct storeKW : TAO_PEGTL_KEYWORD("STORE") {};
struct asFileKW : TAO_PEGTL_KEYWORD("AS FILE") {};
struct asFilesKW : TAO_PEGTL_KEYWORD("AS FILES") {};
struct inStreamKW : TAO_PEGTL_KEYWORD("AS STREAM") {};
struct groupedByKW : TAO_PEGTL_KEYWORD("GROUPED BY") {};
#endif
}  // namespace joda::queryparsing::grammar

struct customASId : tao::pegtl::identifier {};
struct customASPartKW : TAO_PEGTL_KEYWORD("AS") {};
struct customAS
    : tao::pegtl::seq<customASPartKW,
                      tao::pegtl::pad<customASId, tao::pegtl::space>> {};

#include "Literals.h"

namespace joda::queryparsing::grammar {
/*
 * Store Literals
 */
struct storeFileName : tao::pegtl::plus<tao::pegtl::not_one<'"'>> {};
struct storeFilesName : tao::pegtl::plus<tao::pegtl::not_one<'"'>> {};
struct storeIdent : tao::pegtl::identifier {};

struct storeFileliteral
    : tao::pegtl::seq<tao::pegtl::must<tao::pegtl::one<'"'>>, storeFileName,
                      tao::pegtl::must<tao::pegtl::one<'"'>>> {};
struct storeFilesliteral
    : tao::pegtl::seq<tao::pegtl::must<tao::pegtl::one<'"'>>, storeFilesName,
                      tao::pegtl::must<tao::pegtl::one<'"'>>> {};
struct storeAsFileCommand
    : tao::pegtl::seq<tao::pegtl::pad<asFileKW, tao::pegtl::space>,
                      tao::pegtl::pad<tao::pegtl::must<storeFileliteral>,
                                      tao::pegtl::space>> {};
struct storeAsFilesCommand
    : tao::pegtl::seq<tao::pegtl::pad<asFilesKW, tao::pegtl::space>,
                      tao::pegtl::pad<tao::pegtl::must<storeFilesliteral>,
                                      tao::pegtl::space>> {};

// TODO Empty/no parameter
// TODO Error reporting on unknown function

struct storeCutomParam : tao::pegtl::plus<tao::pegtl::not_one<'"'>> {};

struct storeAscustomliteral
    : tao::pegtl::seq<tao::pegtl::must<tao::pegtl::one<'"'>>, storeCutomParam,
                      tao::pegtl::must<tao::pegtl::one<'"'>>> {};

struct storeAsCustomCommand
    : tao::pegtl::seq<tao::pegtl::pad<customAS, tao::pegtl::space>,
                      tao::pegtl::pad<tao::pegtl::must<storeAscustomliteral>,
                                      tao::pegtl::space>> {};

struct storeAsGroupCommand
    : tao::pegtl::seq<
          tao::pegtl::pad<groupedByKW, tao::pegtl::space>,
          tao::pegtl::pad<tao::pegtl::must<groupIdent>, tao::pegtl::space>> {};
struct storeCommand
    : tao::pegtl::opt<tao::pegtl::pad<storeKW, tao::pegtl::space>,
                      tao::pegtl::must<tao::pegtl::sor<
                          inStreamKW, storeAsGroupCommand, storeAsFileCommand,
                          storeAsFilesCommand, storeAsCustomCommand,
                          tao::pegtl::pad<storeIdent, tao::pegtl::space>>>> {};

}  // namespace joda::queryparsing::grammar
#endif  // JODA_STORESTATEMENT_H
