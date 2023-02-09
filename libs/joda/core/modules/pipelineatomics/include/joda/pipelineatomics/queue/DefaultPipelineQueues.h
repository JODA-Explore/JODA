#ifndef JODA_PIPELINE_DEFAULT_IOQUEUE_H
#define JODA_PIPELINE_DEFAULT_IOQUEUE_H

#include "PipelineIOQueue.h"
#include "PipelineQueue.h"
namespace joda::queryexecution::pipeline::queue {
/*
 Queue Sizes
*/

template <class T>
concept QueueSizeTrait = requires {
  { T::BulkSize } -> std::convertible_to<size_t>;  // Queue Block Size
};

struct MinimalQueueTrait {
  static constexpr size_t BulkSize = 2;
};

struct SmallQueueTrait {
  static constexpr size_t BulkSize = 32;
};

struct MediumQueueTrait {
  static constexpr size_t BulkSize = 128;
};

struct LargeQueueTrait {
  static constexpr size_t BulkSize = 512;
};

/*
 IO Queues
*/

template <class T>
concept ITrait = requires {
  { T::InputBulk } -> std::convertible_to<size_t>;  // Input Bulk
  typename T::Input;                                // Input  Type
  typename T::InputQueue;                           // Input Queue

};

template <class T>
concept OTrait = requires {
  { T::OutputBulk } -> std::convertible_to<size_t>;  // Output Bulk
  typename T::Output;                                // Output  Type
  typename T::OutputQueue;                           // Output Queue
};

template <class Element, QueueSizeTrait QueueSize = LargeQueueTrait>
struct InputQueueTrait {
  static constexpr size_t InputBulkSize = QueueSize::BulkSize;
  typedef Element Input;
  typedef PipelineIOQueue<Input, InputBulkSize> InputQueue;

};

template <class Element, QueueSizeTrait QueueSize = LargeQueueTrait>
struct OutputQueueTrait {
  static constexpr size_t OutputBulkSize = QueueSize::BulkSize;
  typedef Element Output;
  typedef PipelineIOQueue<Output, OutputBulkSize> OutputQueue;
};

/*
Default Input/Output
*/
typedef InputQueueTrait<NoOpt, MinimalQueueTrait> NoOptInputQueue;

typedef OutputQueueTrait<NoOpt, MinimalQueueTrait> NoOptOutputQueue;

}  // namespace joda::queryexecution::pipeline::queue

#endif  // JODA_PIPELINE_DEFAULT_IOQUEUE_H