//
// Created by Nico on 22/02/2019.
//

#ifndef JODA_PARSERFLAGS_H
#define JODA_PARSERFLAGS_H

#include <joda/concurrency/Queue.h>
#include <joda/document/IOrigin.h>
#include <joda/document/IDPositionOrigin.h>

#define JODA_PARSER_FLAG_T JODA_FLAG_T
enum class ParserFlag : JODA_PARSER_FLAG_T
{
  NONE = 0,
  JSON = (1u << 0u),
  TEXT = (1u << 1u),
  MEMORY = (1u << 2u),
  STREAM = (1u << 3u),

  //XML = (1 << 1)
  //URL = (1 << 2),
  //..
};


constexpr enum ParserFlag operator |( const enum ParserFlag selfValue, const enum ParserFlag inValue )
{
  return (enum ParserFlag)(JODA_PARSER_FLAG_T(selfValue) | JODA_PARSER_FLAG_T(inValue));
}

constexpr enum ParserFlag operator&(const enum ParserFlag selfValue, const enum ParserFlag inValue) {
  return (enum ParserFlag) (JODA_PARSER_FLAG_T(selfValue) & JODA_PARSER_FLAG_T(inValue));
}

namespace std {
template<>
struct hash<ParserFlag> {
  typedef ParserFlag argument_type;
  typedef size_t result_type;

  result_type operator()(const argument_type &x) const {
    using type = typename std::underlying_type<argument_type>::type;
    return std::hash<type>()(static_cast<type>(x));
  }
};
}

inline std::ostream& operator<<(std::ostream & os, const ParserFlag & flag){
  if(flag == ParserFlag::NONE){
    os << "NONE";
  }else{
    bool first = true;
    if((flag & ParserFlag::JSON) == ParserFlag::JSON){
      first = false;
      os << "JSON";
    }
    if((flag & ParserFlag::TEXT) == ParserFlag::TEXT){
      if(!first)os << "|";
      first = false;
      os << "TEXT";
    }
    if ((flag & ParserFlag::MEMORY) == ParserFlag::MEMORY) {
      if (!first)os << "|";
      first = false;
      os << "MEMORY";
    }
    if ((flag & ParserFlag::STREAM) == ParserFlag::STREAM) {
      if (!first)os << "|";
      first = false;
      os << "STREAM";
    }
  }
  return os;
}



/*
 * Used ParserFlags
 */
#define JODA_JSON_TEXT_PARSER_FLAG (ParserFlag::JSON|ParserFlag::TEXT|ParserFlag::MEMORY)
#define JODA_JSON_TEXT_STREAM_PARSER_FLAG (ParserFlag::JSON|ParserFlag::TEXT|ParserFlag::STREAM)



/*
 * Queue Traits
 */
//Basic queue trait
template <ParserFlag R_FLAGS>
struct JODA_PARSER_QUEUE {
  JODA_PARSER_QUEUE(JODA_PARSER_QUEUE& q) = delete; //Delete copy contructor
};

struct LargeQueueBlocks : public moodycamel::ConcurrentQueueDefaultTraits
{
  static const size_t BLOCK_SIZE = 256;		// Use bigger blocks
};
//JSON Text queue
template<>
struct JODA_PARSER_QUEUE<JODA_JSON_TEXT_PARSER_FLAG> {
  typedef std::pair<std::unique_ptr<IOrigin>,std::string> payload_t;
  typedef JODA_SHARED_QUEUE<payload_t,(JODA_FLAG_T)JODA_JSON_TEXT_PARSER_FLAG,LargeQueueBlocks> queue_t;

  static constexpr bool hasFlag(ParserFlag flag) { return (JODA_JSON_TEXT_PARSER_FLAG) == flag; }
  static constexpr ParserFlag getFlag() {return JODA_JSON_TEXT_PARSER_FLAG;}

  static std::unique_ptr<queue_t> getQueue(){
    return std::make_unique<queue_t>();
  }
  static std::unique_ptr<queue_t> getQueue(size_t minCapacity, size_t maxExplicitProducers, size_t maxImplicitProducers = 0) {
    return std::make_unique<queue_t>(minCapacity,maxExplicitProducers,maxImplicitProducers);
  }
};

//JSON Text Stream queue
template<>
struct JODA_PARSER_QUEUE<JODA_JSON_TEXT_STREAM_PARSER_FLAG> {
  typedef std::pair<std::unique_ptr<IDPositionOrigin>, std::unique_ptr<std::basic_istream<char>>> payload_t;
  typedef JODA_SHARED_QUEUE<payload_t,(JODA_FLAG_T)JODA_JSON_TEXT_STREAM_PARSER_FLAG,LargeQueueBlocks> queue_t;

  static constexpr bool hasFlag(ParserFlag flag) { return (JODA_JSON_TEXT_STREAM_PARSER_FLAG) == flag; }

  static constexpr ParserFlag getFlag() { return JODA_JSON_TEXT_STREAM_PARSER_FLAG; }

  static std::unique_ptr<queue_t> getQueue(){
    return std::make_unique<queue_t>();
  }

  static std::unique_ptr<queue_t> getQueue(size_t minCapacity,
                                           size_t maxExplicitProducers,
                                           size_t maxImplicitProducers = 0) {
    return std::make_unique<queue_t>(minCapacity, maxExplicitProducers, maxImplicitProducers);
  }
};


/*
 * Used Parser Queues
 */
typedef JODA_PARSER_QUEUE<JODA_JSON_TEXT_PARSER_FLAG> JsonTextParserQueue;
typedef JODA_PARSER_QUEUE<JODA_JSON_TEXT_STREAM_PARSER_FLAG> JsonTextStreamParserQueue;



#endif //JODA_PARSERFLAGS_H
