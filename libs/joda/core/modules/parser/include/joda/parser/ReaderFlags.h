//
// Created by Nico on 22/02/2019.
//

#ifndef JODA_READERFLAGS_H
#define JODA_READERFLAGS_H

#include <joda/concurrency/Queue.h>




#define JODA_READER_FLAG_T JODA_FLAG_T
enum class ReaderFlag : JODA_READER_FLAG_T
{
  NONE = 0,
  JSON = (1u << 0u),
  FILE = (1u << 1u),
  LINESEPERATED = (1u << 2u),
  BEAUTIFUL = (1u << 3u),
  URL = (1u << 4u),

  //URL = (1 << 2),
  //..
};
constexpr enum ReaderFlag operator |( const enum ReaderFlag selfValue, const enum ReaderFlag inValue )
{
  return (enum ReaderFlag)(JODA_READER_FLAG_T(selfValue) | JODA_READER_FLAG_T(inValue));
}

constexpr enum ReaderFlag operator&(const enum ReaderFlag selfValue, const enum ReaderFlag inValue) {
  return (enum ReaderFlag) (JODA_READER_FLAG_T(selfValue) & JODA_READER_FLAG_T(inValue));
}

namespace std {
template<>
struct hash<ReaderFlag> {
  typedef ReaderFlag argument_type;
  typedef size_t result_type;

  result_type operator()(const argument_type &x) const {
    using type = typename std::underlying_type<argument_type>::type;
    return std::hash<type>()(static_cast<type>(x));
  }
};
}
inline std::ostream& operator<<(std::ostream & os, const ReaderFlag & flag){
  if(flag == ReaderFlag::NONE){
    os << "NONE";
  }else{
    bool first = true;
    if((flag & ReaderFlag::JSON) == ReaderFlag::JSON){
      first = false;
      os << "JSON";
    }
    if((flag & ReaderFlag::FILE) == ReaderFlag::FILE){
      if(!first)os << "|";
      first = false;
      os << "FILE";
    }
    if((flag & ReaderFlag::LINESEPERATED) == ReaderFlag::LINESEPERATED){
      if(!first)os << "|";
      first = false;
      os << "LINESEPERATED";
    }
    if((flag & ReaderFlag::BEAUTIFUL) == ReaderFlag::BEAUTIFUL){
      if(!first)os << "|";
      first = false;
      os << "BEAUTIFUL";
    }
  }
  return os;
}

/*
 * Used ReaderFlags
 */
#define JODA_JSON_FILE_LINESEPERATED_READER_FLAG (ReaderFlag::JSON|ReaderFlag::FILE|ReaderFlag::LINESEPERATED)
#define JODA_JSON_FILE_BEAUTIFIED_READER_FLAG (ReaderFlag::JSON|ReaderFlag::FILE|ReaderFlag::BEAUTIFUL)
#define JODA_JSON_URL_READER_FLAG (ReaderFlag::JSON|ReaderFlag::URL)



/*
 * Queue Traits
 */
//Basic queue trait
template <ReaderFlag R_FLAGS>
struct JODA_READER_QUEUE {
  JODA_READER_QUEUE(JODA_READER_QUEUE& q) = delete; //Delete copy contructor
};

struct SampleFile{
  double sample;
  std::string file;
};

//JSON File queue
template<>
struct JODA_READER_QUEUE<JODA_JSON_FILE_LINESEPERATED_READER_FLAG> {
  typedef SampleFile payload_t;
  typedef JODA_SHARED_QUEUE<payload_t,(JODA_FLAG_T)JODA_JSON_FILE_LINESEPERATED_READER_FLAG> queue_t;

  static constexpr bool hasFlag(ReaderFlag flag) { return (JODA_JSON_FILE_LINESEPERATED_READER_FLAG) == flag; }

  static constexpr ReaderFlag getFlag() {return JODA_JSON_FILE_LINESEPERATED_READER_FLAG;}

  static std::unique_ptr<queue_t> getQueue(){
    return std::make_unique<queue_t>();
  }
  static std::unique_ptr<queue_t> getQueue(size_t minCapacity, size_t maxExplicitProducers, size_t maxImplicitProducers = 0) {
    return std::make_unique<queue_t>(minCapacity,maxExplicitProducers,maxImplicitProducers);
  }
};

//JSON File queue
template<>
struct JODA_READER_QUEUE<JODA_JSON_FILE_BEAUTIFIED_READER_FLAG> {
  typedef SampleFile payload_t;
  typedef JODA_SHARED_QUEUE<payload_t,(JODA_FLAG_T)JODA_JSON_FILE_BEAUTIFIED_READER_FLAG> queue_t;

  static constexpr bool hasFlag(ReaderFlag flag) { return (JODA_JSON_FILE_BEAUTIFIED_READER_FLAG) == flag; }
  static constexpr ReaderFlag getFlag() {return JODA_JSON_FILE_BEAUTIFIED_READER_FLAG;}

  static std::unique_ptr<queue_t> getQueue(){
    return std::make_unique<queue_t>();
  }
  static std::unique_ptr<queue_t> getQueue(size_t minCapacity, size_t maxExplicitProducers, size_t maxImplicitProducers = 0) {
    return std::make_unique<queue_t>(minCapacity,maxExplicitProducers,maxImplicitProducers);
  }
};

struct URLPayload {
  std::string url;

};

//JSON URL queue
template<>
struct JODA_READER_QUEUE<JODA_JSON_URL_READER_FLAG> {
  typedef URLPayload payload_t;
  typedef JODA_SHARED_QUEUE<payload_t,(JODA_FLAG_T)JODA_JSON_URL_READER_FLAG> queue_t;

  static constexpr bool hasFlag(ReaderFlag flag) { return (JODA_JSON_URL_READER_FLAG) == flag; }

  static constexpr ReaderFlag getFlag() { return JODA_JSON_URL_READER_FLAG; }

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
 * Used Reader Queues
 */
typedef JODA_READER_QUEUE<JODA_JSON_FILE_LINESEPERATED_READER_FLAG> JsonFileSeperatedReaderQueue;
typedef JODA_READER_QUEUE<JODA_JSON_FILE_BEAUTIFIED_READER_FLAG> JsonFileBeautifiedReaderQueue;
typedef JODA_READER_QUEUE<JODA_JSON_URL_READER_FLAG> JsonURLReaderQueue;


#endif //JODA_READERFLAGS_H
