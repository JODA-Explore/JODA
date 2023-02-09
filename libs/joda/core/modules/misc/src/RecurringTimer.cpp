//
// Created by Nico Schäfer on 9/19/17.
//

#include "../include/joda/misc/RecurringTimer.h"
#include <glog/logging.h>
#include "../include/joda/misc/Timer.h"
#include <iomanip>

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


std::string RecurringTimer::toHumanDuration() const {
  auto input_ms = std::chrono::duration_cast<std::chrono::milliseconds>(totalDur);
  return Timer::toHumanDuration(input_ms);
}

void RecurringTimer::addTimer(const RecurringTimer& other){
  totalDur += other.totalDur;
}

void RecurringTimer::addTimer(const std::chrono::milliseconds& other){
  totalDur += other;
}

std::chrono::milliseconds RecurringTimer::getDuration() const{
  return std::chrono::duration_cast<std::chrono::milliseconds>(totalDur);
}



ScopeTimer RecurringTimer::timeScope() {
  return ScopeTimer(this);
}


ScopeTimer::ScopeTimer(RecurringTimer* timer) : timer(timer) {
  timer->start();
}

ScopeTimer::~ScopeTimer() {
  timer->stop();
}