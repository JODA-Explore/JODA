//
// Created by Nico Schäfer on 9/19/17.
//

#include "../include/joda/misc/RecurringTimer.h"
#include <glog/logging.h>
#include "../include/joda/misc/Timer.h"
std::chrono::time_point<std::chrono::system_clock> RecurringTimer::now() {
  return std::chrono::high_resolution_clock::now();
}
void RecurringTimer::start() {
#ifdef JODA_TIMING_ENABLED
  startTime = now();
#endif
}
void RecurringTimer::stop() {
  totalDur +=
      std::chrono::duration_cast<std::chrono::microseconds>(now() - startTime);
}

void RecurringTimer::log(const std::string& what,
                         const std::string& part) const {
#ifdef JODA_TIMING_ENABLED
  std::string duration = "ms";
  double diff =
      std::chrono::duration_cast<std::chrono::milliseconds>(totalDur).count();
  if (diff > 1000) {
    duration = "s";
    diff /= 1000;
  }
  LOG(INFO) << '[' << part << "] " << what << " took " << diff << ' '
            << duration;
#endif
}

double RecurringTimer::durationSeconds() const {
#ifndef JODA_TIMING_ENABLED
  return 0;
#endif
  auto ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(totalDur).count();
  return static_cast<double>(ms) / 1000.0;
}
