//
// Created by Nico on 21/02/2019.
//

#ifndef JODA_LOADSTATEMENT_H
#define JODA_LOADSTATEMENT_H

#include "../../../../../../../extern/PEGTL/include/tao/pegtl.hpp"

namespace joda::queryparsing::grammar {
/*
 * Forward Declaration Actions
 */
template <typename Rule>
struct loadAction : tao::pegtl::nothing<Rule> {};

/*
 * Keywords
 */
#ifndef __CLION_IDE__  // Prevent lag from expanding all macros
struct loadKW : TAO_PEGTL_KEYWORD("LOAD") {};
struct fromFileKW : TAO_PEGTL_KEYWORD("FROM FILE") {};
struct fromFilesKW : TAO_PEGTL_KEYWORD("FROM FILES") {};
struct fromGroupedKW : TAO_PEGTL_KEYWORD("FROM GROUPED") {};
struct fromURLKW : TAO_PEGTL_KEYWORD("FROM URL") {};
struct sampleKW : TAO_PEGTL_KEYWORD("SAMPLE") {};
struct lineSeperatedKW : TAO_PEGTL_KEYWORD("LINESEPARATED") {};
struct fromStreamKW : TAO_PEGTL_KEYWORD("FROM STREAM") {};
#endif

// TODO Empty/no parameter
// TODO Error reporting on unknown function
struct customFromId : tao::pegtl::identifier {};
struct customFromPartKW : TAO_PEGTL_KEYWORD("FROM") {};
struct customFrom : tao::pegtl::seq<customFromPartKW,tao::pegtl::pad<customFromId, tao::pegtl::space>> {};


}  // namespace joda::queryparsing::grammar
#include "Literals.h"
namespace joda::queryparsing::grammar {
struct loadFileName : tao::pegtl::star<tao::pegtl::not_one<'"'>> {};
struct loadIdent : tao::pegtl::identifier {};
struct sampleNum : tao::pegtl::seq<tao::pegtl::one<'0'>, tao::pegtl::one<'.'>,
                                   tao::pegtl::plus<tao::pegtl::digit>> {};

struct sampleCommand
    : tao::pegtl::pad<
          tao::pegtl::seq<sampleKW, tao::pegtl::pad<tao::pegtl::must<sampleNum>,
                                                    tao::pegtl::space>>,
          tao::pegtl::space> {};

struct loadFilesKWs : tao::pegtl::sor<fromFilesKW, fromFileKW, fromURLKW, customFrom> {};

struct loadFileLiteralStart : tao::pegtl::one<'"'> {};
struct loadFileLiteralEnd : tao::pegtl::one<'"'> {};
struct loadFilesliteral
    : tao::pegtl::seq<tao::pegtl::must<loadFileLiteralStart>,
                      tao::pegtl::must<loadFileName>,
                      tao::pegtl::must<loadFileLiteralEnd>> {};
struct loadFilesCommand
    : tao::pegtl::seq<tao::pegtl::pad<loadFilesKWs, tao::pegtl::space>,
                      tao::pegtl::pad<tao::pegtl::must<loadFilesliteral>,
                                      tao::pegtl::space>> {};

struct loadStreamCommand: tao::pegtl::pad<fromStreamKW, tao::pegtl::space> {};

struct loadFilesLineSeperatedCommand
    : tao::pegtl::pad<lineSeperatedKW, tao::pegtl::space> {};

struct loadAndSampleCommand
    : tao::pegtl::seq<tao::pegtl::sor<loadStreamCommand,loadFilesCommand>,
                      tao::pegtl::opt<loadFilesLineSeperatedCommand>,
                      tao::pegtl::opt<sampleCommand>> {};

struct loadGroupedCommand
    : tao::pegtl::seq<
          tao::pegtl::pad<fromGroupedKW, tao::pegtl::space>,
          tao::pegtl::pad<tao::pegtl::must<groupIdent>, tao::pegtl::space>> {};

struct loadImportSource
    : tao::pegtl::sor<loadAndSampleCommand, loadGroupedCommand> {
};  // FROM ....
// FROM ..., FROM ..., ...
struct loadImportSources
    : tao::pegtl::seq<
          loadImportSource,
          tao::pegtl::opt<tao::pegtl::seq<
              tao::pegtl::pad<tao::pegtl::one<','>, tao::pegtl::space>,
              loadImportSources>>> {};

struct loadCommand
    : tao::pegtl::seq<
          tao::pegtl::pad<tao::pegtl::must<loadKW>, tao::pegtl::space>,
          tao::pegtl::sor<loadImportSources,
            tao::pegtl::seq<tao::pegtl::pad<loadIdent, tao::pegtl::space>,tao::pegtl::opt<loadImportSources>>
           >
          > {};

}  // namespace joda::queryparsing::grammar
#endif  // JODA_LOADSTATEMENT_H
