//
// Created by Nico Schäfer on 20/08/18.
//
#ifndef __CLION_IDE__ //Prevent IDE lag from expanding all macros
#ifndef JODA_FUNCTIONWRAPPER_H
#define JODA_FUNCTIONWRAPPER_H
#include <tao/pegtl.hpp>

namespace joda::queryparsing::grammar {

template<typename Rule>
struct functionAction
    : tao::pegtl::nothing<Rule> {
};




/*
 * FCLASS:    Classname of Function IValueProvider
 * FSTRUCT:   (Unique) Name of kexword-struct
 * FKEXWORD:  (Unique) Keyword name in querylanguage
 */
#define REGISTER_FUNCTION(FCLASS,FSTRUCT,FKEYWORD) \
struct FSTRUCT : TAOCPP_PEGTL_KEYWORD(FKEYWORD) { \
  static std::string toString()  {return FKEYWORD;} \
};\
template<> \
struct functionAction<FSTRUCT> { \
  static void apply0(functionState &state) { \
    state.factory = &FCLASS::_FACTORY;\
  } \
};
}

#include "states/States.h"
#include <joda/query/values/ArraySizeProvider.h>
#include <joda/query/values/FileNameProvider.h>
#include <joda/query/values/FilePosProvider.h>
#include <joda/query/values/IDProvider.h>
#include <joda/query/values/INListProvider.h>
#include <joda/query/values/IsXBoolProvider.h>
#include <joda/query/values/ListAttributesProvider.h>
#include <joda/query/values/RegexExtractProvider.h>
#include <joda/query/values/RegexProvider.h>
#include <joda/query/values/RegexReplaceProvider.h>
#include <joda/query/values/SeqNumberProvider.h>
#include <joda/query/values/TypeProvider.h>
#include <joda/query/values/BinaryNumberProvider.h>
#include <joda/query/values/UnaryNumberProvider.h>
#include <joda/query/values/UnaryStringProvider.h>
#include <joda/query/values/BinaryStringProvider.h>
#include <joda/query/values/ConstantNumber.h>

namespace joda::queryparsing::grammar {
REGISTER_FUNCTION(query::ArraySizeProvider,func_kw_arrSize,"SIZE")
REGISTER_FUNCTION(query::FileNameProvider,func_kw_FILE,"FILENAME")
REGISTER_FUNCTION(query::FilePosProvider<true>,func_kw_FILEPOSSTART,"FILEPOSSTART")
REGISTER_FUNCTION(query::FilePosProvider<false>,func_kw_FILEPOSEND,"FILEPOSEND")
REGISTER_FUNCTION(query::IDProvider,func_kw_ID,"ID")
REGISTER_FUNCTION(query::INListProvider,func_kw_IN,"IN")
REGISTER_FUNCTION(query::IsXBoolProvider<query::IV_Array>,func_kw_isArray,"ISARRAY")
REGISTER_FUNCTION(query::IsXBoolProvider<query::IV_Object>,func_kw_isObject,"ISOBJECT")
REGISTER_FUNCTION(query::IsXBoolProvider<query::IV_Number>,func_kw_isNumber,"ISNUMBER")
REGISTER_FUNCTION(query::IsXBoolProvider<query::IV_Bool>,func_kw_isBool,"ISBOOL")
REGISTER_FUNCTION(query::IsXBoolProvider<query::IV_String>,func_kw_isString,"ISSTRING")
REGISTER_FUNCTION(query::IsXBoolProvider<query::IV_Null>,func_kw_isNull,"ISNULL")
REGISTER_FUNCTION(query::IsXBoolProvider<query::IV_Any>,func_kw_exists,"EXISTS")
REGISTER_FUNCTION(query::ListAttributesProvider,func_kw_LISTATTRIBUTES,"LISTATTRIBUTES")
REGISTER_FUNCTION(query::RegexExtractProvider,func_kw_REGEX_EXTRACT,"REGEX_EXTRACT")
REGISTER_FUNCTION(query::RegexProvider,func_kw_REGEX,"REGEX")
REGISTER_FUNCTION(query::RegexReplaceProvider,func_kw_REGEX_REPLACE,"REGEX_REPLACE")
REGISTER_FUNCTION(query::SCONTAINSProvider,func_kw_sContains,"SCONTAINS")
REGISTER_FUNCTION(query::STARTSWITHProvider,func_kw_STARTSWITH,"STARTSWITH")
REGISTER_FUNCTION(query::SeqNumberProvider,func_kw_SEQNUM,"SEQNUM")
REGISTER_FUNCTION(query::TypeProvider,func_kw_TYPE,"TYPE")
REGISTER_FUNCTION(query::SumProvider,func_kw_SUMFUNK,"SUM")
REGISTER_FUNCTION(query::SubtractProvider,func_kw_SUBFUNK,"SUB")
REGISTER_FUNCTION(query::ProductProvider,func_kw_PRODFUNK,"PROD")
REGISTER_FUNCTION(query::DivProvider,func_kw_DIVFUNK,"DIV")
REGISTER_FUNCTION(query::ModuloProvider,func_kw_MODFUNK,"MOD")
REGISTER_FUNCTION(query::PowerProvider,func_kw_POWFUNK,"POW")
REGISTER_FUNCTION(query::Atan2Provider,func_kw_ATAN2FUNK,"ATAN2")
REGISTER_FUNCTION(query::AbsProvider,func_kw_ABSFUNK,"ABS")
REGISTER_FUNCTION(query::RoundProvider,func_kw_ROUNDFUNK,"ROUND")
REGISTER_FUNCTION(query::SqrtProvider,func_kw_SQRTFUNK,"SQRT")
REGISTER_FUNCTION(query::CeilProvider,func_kw_CEILFUNK,"CEIL")
REGISTER_FUNCTION(query::FloorProvider,func_kw_FLOORFUNK,"FLOOR")
REGISTER_FUNCTION(query::DegreesProvider,func_kw_DEGREESFUNK,"DEGREES")
REGISTER_FUNCTION(query::RadiansProvider,func_kw_RADIANSFUNK,"RADIANS")
REGISTER_FUNCTION(query::AcosProvider,func_kw_ACOSFUNK,"ACOS")
REGISTER_FUNCTION(query::AsinProvider,func_kw_ASINFUNK,"ASIN")
REGISTER_FUNCTION(query::AtanProvider,func_kw_ATANFUNK,"ATAN")
REGISTER_FUNCTION(query::CosProvider,func_kw_COSFUNK,"COS")
REGISTER_FUNCTION(query::SinProvider,func_kw_SINFUNK,"SIN")
REGISTER_FUNCTION(query::TanProvider,func_kw_TANFUNK,"TAN")
REGISTER_FUNCTION(query::TruncProvider,func_kw_TRUNCFUNK,"TRUNC")
REGISTER_FUNCTION(query::LenProvider,func_kw_LENFUNK,"LEN")
REGISTER_FUNCTION(query::ConcatProvider,func_kw_CONCATFUNK,"CONCAT")
REGISTER_FUNCTION(query::UpperProvider,func_kw_UPPERFUNK,"UPPER")
REGISTER_FUNCTION(query::LowerProvider,func_kw_LOWERFUNK,"LOWER")
REGISTER_FUNCTION(query::LtrimProvider,func_kw_LTRIMFUNK,"LTRIM")
REGISTER_FUNCTION(query::RtrimProvider,func_kw_RTRIMFUNK,"RTRIM")
REGISTER_FUNCTION(query::PiProvider,func_kw_PIFUNK,"PI")
REGISTER_FUNCTION(query::NowProvider,func_kw_NOWFUNK,"NOW")

template< typename... Rules >
struct stringableSOR : tao::pegtl::sor<Rules...>{
  template<typename First, typename... Rest>
  static constexpr auto toString() { return First::toString() + " " + stringableSOR<Rest...>::toString(); }

  static constexpr auto toString() { return toString<Rules...>(); }
};

template <>
struct stringableSOR<>{
  static constexpr auto toString() { return ""; }
};

struct functionKWs : stringableSOR<func_kw_exists,
                                     func_kw_isArray,
                                     func_kw_isObject,
                                     func_kw_isNumber,
                                     func_kw_isBool,
                                     func_kw_isString,
                                     func_kw_isNull,
                                     func_kw_IN,
                                     func_kw_sContains,
                                     func_kw_STARTSWITH,
                                     func_kw_arrSize,
                                     func_kw_ID,
                                     func_kw_FILE,
                                     func_kw_FILEPOSSTART,
                                     func_kw_FILEPOSEND,
                                     func_kw_SEQNUM,
                                     func_kw_LISTATTRIBUTES,
                                     func_kw_TYPE,
                                     func_kw_REGEX,
                                     func_kw_REGEX_EXTRACT,
                                     func_kw_REGEX_REPLACE,
                                     func_kw_SUMFUNK,
                                     func_kw_SUBFUNK,
                                     func_kw_PRODFUNK,
                                     func_kw_MODFUNK,
                                     func_kw_POWFUNK,
                                     func_kw_ATAN2FUNK, //func_kw_ATAN2FUNK has to be before func_kw_ATANFUNK
                                     func_kw_ABSFUNK,
                                     func_kw_ROUNDFUNK,
                                     func_kw_SQRTFUNK,
                                     func_kw_CEILFUNK,
                                     func_kw_FLOORFUNK,
                                     func_kw_DEGREESFUNK,
                                     func_kw_RADIANSFUNK,
                                     func_kw_ACOSFUNK,
                                     func_kw_ASINFUNK,
                                     func_kw_ATANFUNK,
                                     func_kw_COSFUNK,
                                     func_kw_SINFUNK,
                                     func_kw_TANFUNK,
                                     func_kw_TRUNCFUNK,
                                     func_kw_LENFUNK,
                                     func_kw_UPPERFUNK,
                                     func_kw_LOWERFUNK,
                                     func_kw_LTRIMFUNK,
                                     func_kw_RTRIMFUNK,
                                     func_kw_PIFUNK,
                                     func_kw_NOWFUNK,
                                     func_kw_DIVFUNK
> {

};

}
#include "grammar/Grammar.h"

#endif //JODA_FUNCTIONWRAPPER_H
#endif