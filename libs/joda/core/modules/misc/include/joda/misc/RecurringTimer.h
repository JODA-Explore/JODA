//
// Created by Nico Schäfer on 9/19/17.
//

#ifndef JODA_RECURRINGTIMER_H
#define JODA_RECURRINGTIMER_H

#include <chrono>
#include <string>

class RecurringTimer;

/**
 * @brief Times the execution of a function or scope
 *
 */
class ScopeTimer {
 public:
  /**
   * @brief Construct a new ScopeTimer object
   *
   * @param timer
   */
  ScopeTimer(RecurringTimer* timer);

  /**
   * @brief Destroy the ScopeTimer object
   *
   */
  ~ScopeTimer();

 private:
  RecurringTimer* timer;
};

/**
 * An cumulative timer that can be started and stopped multiple times and summs
 * up each runtime.
 */
class RecurringTimer {
 public:
 virtual ~RecurringTimer() = default;
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
  void log(const std::string& what, const std::string& part) const;

  /**
   * @brief Adds the runtime of another recurring timer
   * 
   * @param other The timer to add the runtime from
   */
  void addTimer(const RecurringTimer& other);

  /**
   * @brief Adds a generic runtime
   * 
   * @param other The duration to add the runtime from
   */
  void addTimer(const std::chrono::milliseconds& other);

  /**
   * Returns the total duration of the timer in seconds.
   * @return the total duration of the timer in seconds.
   */
  double durationSeconds() const;

  /**
   * @brief Get the duration of the timer
   * 
   * @return std::chrono::milliseconds The duration of the timer in ms
   */
  std::chrono::milliseconds getDuration() const;

  /**
   * Creates a human readable string representation of the duration in the
   * format: "xh xm xs xms"
   * @return human readable string representation of the duration
   */
  std::string toHumanDuration() const;

  /**
   * @brief Creates a scope timer using the current timer
   *
   * @return ScopeTimer
   */
  ScopeTimer timeScope();

 protected:
  std::chrono::time_point<std::chrono::system_clock> now();
  std::chrono::time_point<std::chrono::system_clock> startTime;
  std::chrono::duration<long long, std::micro> totalDur{};
};

#endif  // JODA_RECURRINGTIMER_H
