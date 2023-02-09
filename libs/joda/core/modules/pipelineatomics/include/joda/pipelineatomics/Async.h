#ifndef JODA_PIPELINE_ASYNC_H
#define JODA_PIPELINE_ASYNC_H



namespace joda::queryexecution::pipeline::tasks {
  /**
   * @brief The representation of how many threads the task is expected to run on.
   */
  enum class AsyncType {
    // Synchronous tasks are pipeline breaker and will be executed by the main thread.
    Synchronous,
    // SingleThreaded tasks run in parallel, but use only one thread maximum
    SingleThreaded,
    // MultiThreaded tasks run in parallel with as many tasks as are available
    MultiThreaded
  };



}  // namespace joda::queryexecution::pipeline::tasks

#endif  // JODA_PIPELINE_ASYNC_H