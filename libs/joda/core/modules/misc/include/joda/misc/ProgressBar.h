//(The MIT License)
//
// Copyright (c) 2016 Prakhar Srivastav <prakhar@prakhar.me>
//
// Permission is hereby granted, free of charge, to any person obtaining
//    a copy of this software and associated documentation files (the
//'Software'), to deal in the Software without restriction, including
//    without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
//    permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef JODA_PROGRESSBAR_H
#define JODA_PROGRESSBAR_H

#include <chrono>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <sstream>

template <typename IntType>
class ProgressBar {
 private:
  IntType max_limit;
  IntType ticks = 0;
  const IntType total_ticks;
  const IntType bar_width;
  const char complete_char = '=';
  const char incomplete_char = ' ';
  std::chrono::steady_clock::time_point start_time =
      std::chrono::steady_clock::now();

 public:
  const IntType getBar_width() const { return bar_width; }

 private:
  static void output_time(std::stringstream& ss, double seconds) {
    std::size_t s_in_h(60 * 60), s_in_m(60) /*, m_in_h(60)*/;

    IntType hours_rem = seconds / s_in_h;

    IntType minutes_rem = (seconds - (hours_rem * s_in_h)) / s_in_m;

    IntType seconds_rem =
        seconds - ((hours_rem * s_in_h) + (minutes_rem * s_in_m));

    std::ios init(nullptr), time_fmt(nullptr);
    init.copyfmt(ss);
    ss << std::setfill('0') << std::setw(2);
    time_fmt.copyfmt(ss);
    // hours
    ss.copyfmt(time_fmt);
    ss << hours_rem;
    ss.copyfmt(init);
    // minutes
    ss << ":";
    ss.copyfmt(time_fmt);
    ss << minutes_rem;
    ss.copyfmt(init);
    // seconds
    ss << ":";
    ss.copyfmt(time_fmt);
    ss << seconds_rem;
    // restore format
    ss.copyfmt(init);
  }

 public:
  ProgressBar(IntType total, IntType width, char complete, char incomplete)
      : max_limit(std::numeric_limits<IntType>().max()),
        total_ticks{total},
        bar_width{width},
        complete_char{complete},
        incomplete_char{incomplete} {}

  ProgressBar(IntType total, IntType width)
      : max_limit(std::numeric_limits<IntType>().max()),
        total_ticks{total},
        bar_width{width} {}

  ProgressBar& operator++() {
    if (ticks != max_limit) {
      ++ticks;
    }
    return *this;
  }

  const ProgressBar operator++(int) {
    ProgressBar tmp = *this;
    ++(*this);
    return tmp;
  }

  ProgressBar& operator--() {
    if (ticks != 0) {
      --ticks;
    }
    return *this;
  }

  const ProgressBar operator--(int) {
    ProgressBar tmp = *this;
    --(*this);
    return tmp;
  }

  template <typename NumberType>
  void set_progress(NumberType prog) {
    if (prog >= 0) {
      ticks = std::min(prog, max_limit);
    }
  }

  template <typename NumberType>
  void add_progress(NumberType prog) {
    if (prog < 0) {
      remove_progress(-prog);
    };
    IntType dist_from_limit = max_limit - ticks;
    if (prog <= dist_from_limit) {
      ticks = ticks + prog;
    } else {
      ticks = max_limit;
    }
  }
  template <typename NumberType>
  void remove_progress(NumberType prog) {
    if (prog < 0) {
      add_progress(-prog);
    };
    IntType dist_from_limit = ticks - 0;
    if (prog <= dist_from_limit) {
      ticks = ticks - prog;
    } else {
      ticks = 0;
    }
  }
  std::string toString() const {
    double progress =
        static_cast<double>(ticks) / static_cast<double>(total_ticks);
    IntType pos = (IntType)(bar_width * progress);

    std::chrono::steady_clock::time_point now =
        std::chrono::steady_clock::now();
    auto time_elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time)
            .count();

    double seconds = static_cast<double>(time_elapsed) / 1000.0;
    // double ticks_per_second = static_cast<double>(ticks) / seconds;
    // double remaining_ticks = total_ticks - ticks;
    // double total_seconds_rem = remaining_ticks / ticks_per_second;

    std::stringstream ss;
    ss << "\033[2K"
       << "\r";
    ss << "[";

    for (IntType i = 0; i < bar_width; ++i) {
      if (i < pos)
        ss << complete_char;
      else if (i == pos)
        ss << ">";
      else
        ss << incomplete_char;
    }

    ss << "] " << static_cast<IntType>(progress * 100.0) << "% (";

    ss << ticks << "/" << total_ticks << ") ";

    ProgressBar::output_time(ss, seconds);

    // ss<<" {";

    // ProgressBar::output_time(ss, total_seconds_rem);

    // ss <<" remaining}";

    // ss<<" (" << static_cast<std::size_t>(ticks_per_second) << " t/s)";
    return ss.str();
  }

  void display() const {
    double progress =
        static_cast<double>(ticks) / static_cast<double>(total_ticks);
    IntType pos = (IntType)(bar_width * progress);

    std::chrono::steady_clock::time_point now =
        std::chrono::steady_clock::now();
    auto time_elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time)
            .count();

    double seconds = static_cast<double>(time_elapsed) / 1000.0;
    double ticks_per_second = static_cast<double>(ticks) / seconds;
    double remaining_ticks = total_ticks - ticks;
    double total_seconds_rem = remaining_ticks / ticks_per_second;

    std::stringstream ss;
    ss << "\033[2K"
       << "\r";
    ss << "[";

    for (IntType i = 0; i < bar_width; ++i) {
      if (i < pos)
        ss << complete_char;
      else if (i == pos)
        ss << ">";
      else
        ss << incomplete_char;
    }

    ss << "] " << static_cast<IntType>(progress * 100.0) << "% ";

    ss << ticks << " / " << total_ticks << " ticks ";

    ProgressBar::output_time(ss, seconds);

    ss << " {";

    ProgressBar::output_time(ss, total_seconds_rem);

    ss << " remaining}";

    ss << " (" << static_cast<std::size_t>(ticks_per_second) << " t/s)";

    std::cout << ss.str() << std::flush;
  }

  void done() const {
    display();
    std::cout << std::endl;
  }

  void reset() {
    start_time = std::chrono::steady_clock::now();
    ticks = 0;
  }
};
#endif  // JODA_PROGRESSBAR_H
