//
// Created by Nico Schäfer on 5/31/17.
//

#ifndef JODA_TIMER_H
#define JODA_TIMER_H

#include <chrono>
#include <string>

#define JODA_TIMING_ENABLED

/**
 * A timer that can be started and stopped multiple times and replaces the
 * previous runtime.
 */
class Timer {
 public:
  /**
   * Creates a new instance of the timer and immediately starts it.
   */
  Timer();

  /**
   * Start the timer, the previous runtime (if exists) is replaced.
   */
  void start();

  /**
   * Stopps the timer
   */
  void stop();

  /**
   * Returns the total duration of the timer in seconds.
   * @return the total duration of the timer in seconds.
   */
  double durationSeconds();

  /**
   * Creates a string representation of the duration
   * @return string representation of the duration
   */
  std::string toString();

  /**
   * Loggs the runtime to GLOG with an string tag and description
   * "[<part>] <what> took <time>"
   * @param what the description of what was measured
   * @param what the description of what was measured
   */
  void log(const std::string &what, const std::string &part);

  /**
   * Creates a human readable string representation of the duration in the
   * format: "xh xm xs xms"
   * @return human readable string representation of the duration
   */
  std::string toHumanDuration() const;

 protected:
  std::chrono::time_point<std::chrono::system_clock> now();
  std::chrono::time_point<std::chrono::system_clock> startTime;
  std::chrono::time_point<std::chrono::system_clock> endTime;
};

#endif  // JODA_TIMER_H
