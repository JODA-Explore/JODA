//
// Created by Nico Schäfer on 5/31/17.
//

#include "../include/joda/misc/Timer.h"
#include <string>
#include <glog/logging.h>
#include <iomanip>

Timer::Timer(){
    start();
};

void Timer::start() {
#ifdef JODA_TIMING_ENABLED
    startTime = now();
#endif
}

std::chrono::time_point<std::chrono::system_clock> Timer::now() {
    return  std::chrono::high_resolution_clock::now();
}

void Timer::stop() {
#ifdef JODA_TIMING_ENABLED
    endTime = now();
#endif
}

double Timer::durationSeconds() {
#ifndef JODA_TIMING_ENABLED
    return 0;
#endif
    std::chrono::duration<double> diff = endTime-startTime;
    return diff.count();
}

void Timer::log(const std::string &what, const std::string &part) {
#ifdef JODA_TIMING_ENABLED
    std::string duration = "s";
    double diff = durationSeconds();
    if(diff < 0.1){
        duration = "ms";
        diff *= 1000;
    }
  LOG(INFO) << '[' << part << "] " << what << " took " << diff << ' ' << duration;
#endif
}

std::string Timer::toString() {
#ifndef JODA_TIMING_ENABLED
  return "";
#endif
  return std::to_string(
      (double) std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0);
}

std::string Timer::toHumanDuration() const {
  auto input_ms = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
  auto h = std::chrono::duration_cast<std::chrono::hours>(input_ms);
  input_ms -= h;
  auto m = std::chrono::duration_cast<std::chrono::minutes>(input_ms);
  input_ms -= m;
  auto s = std::chrono::duration_cast<std::chrono::seconds>(input_ms);
  input_ms -= s;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(input_ms);

  auto hc = h.count();
  auto mc = m.count();
  auto sc = s.count();
  auto msc = ms.count();

  std::stringstream ss;
  ss.fill('0');

  if (hc) {
    ss << hc << 'h';
  }
  if (hc || mc) {
    if (hc) { ss << std::setw(2); }
    ss << mc << 'm';
  }
  if (hc || mc || sc) {
    if (hc || mc) { ss << std::setw(2); }
    ss << sc << 's';
  }
  if (hc || mc || sc) { ss << std::setw(2); }
  ss << msc << "ms";

  return ss.str();
}
