//
// Created by Nico on 22/02/2019.
//

#ifndef JODA_CONTAINERFLAGS_H
#define JODA_CONTAINERFLAGS_H

#include <joda/concurrency/Queue.h>
#include "JSONContainer.h"

#define JODA_CONTAINER_FLAG_T JODA_FLAG_T
enum class ContainerFlag : JODA_CONTAINER_FLAG_T {
  NONE = 0,
  JSON = (1u << 0u),
  REF = (1u << 1u),
  ADAPTIVE = (1u << 2u)
  //..
};
constexpr enum ContainerFlag operator|(const enum ContainerFlag selfValue,
                                       const enum ContainerFlag inValue) {
  return (enum ContainerFlag)(JODA_CONTAINER_FLAG_T(selfValue) |
                              JODA_CONTAINER_FLAG_T(inValue));
}

constexpr enum ContainerFlag operator&(const enum ContainerFlag selfValue,
                                       const enum ContainerFlag inValue) {
  return (enum ContainerFlag)(JODA_CONTAINER_FLAG_T(selfValue) &
                              JODA_CONTAINER_FLAG_T(inValue));
}
namespace std {
template <>
struct hash<ContainerFlag> {
  typedef ContainerFlag argument_type;
  typedef size_t result_type;

  result_type operator()(const argument_type& x) const {
    using type = typename std::underlying_type<argument_type>::type;
    return std::hash<type>()(static_cast<type>(x));
  }
};
}  // namespace std

inline std::ostream& operator<<(std::ostream& os, const ContainerFlag& flag) {
  if (flag == ContainerFlag::NONE) {
    os << "NONE";
  } else {
    bool first = true;
    if ((flag & ContainerFlag::JSON) == ContainerFlag::JSON) {
      first = false;
      os << "JSON";
    }
    if ((flag & ContainerFlag::REF) == ContainerFlag::REF) {
      if (!first) os << "|";
      first = false;
      os << "REF";
    }
  }
  return os;
}

/*
 * Used ContainerFlags
 */
#define JODA_JSON_CONTAINER_FLAG (ContainerFlag::JSON)
#define JODA_JSON_CONTAINER_REF_FLAG (ContainerFlag::JSON | ContainerFlag::REF)
#define JODA_JSON_ADAPTIVE_CONTAINER_REF_FLAG (ContainerFlag::JSON | ContainerFlag::ADAPTIVE)

/*
 * Queue Traits
 */
// Basic queue trait
template <ContainerFlag R_FLAGS>
struct JODA_CONTAINER_QUEUE {
  JODA_CONTAINER_QUEUE(JODA_CONTAINER_QUEUE& q) =
      delete;  // Delete copy contructor
};

// JSON Container queue
template <>
struct JODA_CONTAINER_QUEUE<JODA_JSON_CONTAINER_FLAG> {
  typedef std::shared_ptr<JSONContainer> payload_t;
  typedef JODA_SHARED_QUEUE<payload_t, (JODA_FLAG_T)JODA_JSON_CONTAINER_FLAG>
      queue_t;

  static constexpr bool hasFlag(ContainerFlag flag) {
    return (JODA_JSON_CONTAINER_FLAG) == flag;
  }

  static constexpr ContainerFlag getFlag() { return JODA_JSON_CONTAINER_FLAG; }

  static std::unique_ptr<queue_t> getQueue() {
    return std::make_unique<queue_t>();
  }
  static std::unique_ptr<queue_t> getQueue(size_t minCapacity,
                                           size_t maxExplicitProducers,
                                           size_t maxImplicitProducers = 0) {
    return std::make_unique<queue_t>(minCapacity, maxExplicitProducers,
                                     maxImplicitProducers);
  }
};

// JSON Container Reference queue
template <>
struct JODA_CONTAINER_QUEUE<JODA_JSON_CONTAINER_REF_FLAG> {
  typedef JSONContainer* payload_t;
  typedef JODA_SHARED_QUEUE<payload_t,
                            (JODA_FLAG_T)JODA_JSON_CONTAINER_REF_FLAG>
      queue_t;

  static constexpr bool hasFlag(ContainerFlag flag) {
    return (JODA_JSON_CONTAINER_REF_FLAG) == flag;
  }

  static constexpr ContainerFlag getFlag() {
    return JODA_JSON_CONTAINER_REF_FLAG;
  }

  static std::unique_ptr<queue_t> getQueue() {
    return std::make_unique<queue_t>();
  }
  static std::unique_ptr<queue_t> getQueue(size_t minCapacity,
                                           size_t maxExplicitProducers,
                                           size_t maxImplicitProducers = 0) {
    return std::make_unique<queue_t>(minCapacity, maxExplicitProducers,
                                     maxImplicitProducers);
  }
};

// JSON Container Adaptive queue
template <>
struct JODA_CONTAINER_QUEUE<JODA_JSON_ADAPTIVE_CONTAINER_REF_FLAG> {
  typedef std::variant<JSONContainer*,std::unique_ptr<JSONContainer>> payload_t;
  typedef JODA_SHARED_QUEUE<payload_t,
                            (JODA_FLAG_T)JODA_JSON_ADAPTIVE_CONTAINER_REF_FLAG>
      queue_t;

  static constexpr bool hasFlag(ContainerFlag flag) {
    return (JODA_JSON_ADAPTIVE_CONTAINER_REF_FLAG) == flag;
  }

  static constexpr ContainerFlag getFlag() {
    return JODA_JSON_ADAPTIVE_CONTAINER_REF_FLAG;
  }

  static std::unique_ptr<queue_t> getQueue() {
    return std::make_unique<queue_t>();
  }
  static std::unique_ptr<queue_t> getQueue(size_t minCapacity,
                                           size_t maxExplicitProducers,
                                           size_t maxImplicitProducers = 0) {
    return std::make_unique<queue_t>(minCapacity, maxExplicitProducers,
                                     maxImplicitProducers);
  }
};

/*
 * Used Container Queues
 */
typedef JODA_CONTAINER_QUEUE<JODA_JSON_CONTAINER_FLAG> JsonContainerQueue;
typedef JODA_CONTAINER_QUEUE<JODA_JSON_CONTAINER_REF_FLAG>
    JsonContainerRefQueue;
typedef JODA_CONTAINER_QUEUE<JODA_JSON_ADAPTIVE_CONTAINER_REF_FLAG>
    JsonContainerAdaptiveQueue;

#endif  // JODA_CONTAINERFLAGS_H
