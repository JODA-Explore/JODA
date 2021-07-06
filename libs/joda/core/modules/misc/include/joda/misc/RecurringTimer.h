//
// Created by Nico Schäfer on 9/19/17.
//

#ifndef JODA_RECURRINGTIMER_H
#define JODA_RECURRINGTIMER_H

#include <chrono>
#include <string>
/**
 * An cumulative timer that can be started and stopped multiple times and summs
 * up each runtime.
 */
class RecurringTimer {
 public:
  /**
   * Start the timer, has to be done explicitely after initializing the class
   */
  void start();

  /**
   * Stopps the timer, and adds the runtime to the endresult.
   */
  void stop();

  /**
   * Loggs the runtime to GLOG with an string tag and description
   * "[<part>] <what> took <time>"
   * @param what the description of what was measured
   * @param what the description of what was measured
   */
  void log(const std::string &what, const std::string &part) const;

  /**
   * Returns the total duration of the timer in seconds.
   * @return the total duration of the timer in seconds.
   */
  double durationSeconds() const;

 protected:
  std::chrono::time_point<std::chrono::system_clock> now();
  std::chrono::time_point<std::chrono::system_clock> startTime;
  std::chrono::duration<long long, std::micro> totalDur{};
};

#endif  // JODA_RECURRINGTIMER_H
