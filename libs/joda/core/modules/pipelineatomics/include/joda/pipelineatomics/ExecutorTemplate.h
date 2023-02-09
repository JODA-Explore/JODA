#ifndef JODA_PIPELINE_EXECUTOR_TEMPLATE_H
#define JODA_PIPELINE_EXECUTOR_TEMPLATE_H
#include <joda/misc/RecurringTimer.h>

#include <cstdio>
#include <optional>

#include "PipelineIOTask.h"
#include "queue/DefaultPipelineQueues.h"
#include "queue/PipelineIOQueue.h"

namespace joda::queryexecution::pipeline::tasks {

template <typename T>
concept HasInputBulk = requires {
  T::InputBulk;
};

template <typename T>
concept HasNoInputBulk = !HasInputBulk<T>;

template <typename T>
concept HasGetData = requires(T x) {
  x.getData();
};

template <typename T>
concept HasNoGetData = !HasGetData<T>;

template <typename T>
concept HasToString = requires(T x) {
  x.toString();
};

template <typename T>
concept HasNoToString = !HasToString<T>;

template <class Calc, tasks::AsyncType A>
class OExecutor {
 public:
  // NoOpt Input
  typedef queue::NoOptInputQueue I;
  typedef typename I::Input Input;
  typedef typename I::InputQueue InputQueue;
  static constexpr size_t InputBulk = I::InputBulkSize;

  // Output
  typedef typename Calc::O O;
  typedef typename O::Output Output;
  typedef typename O::OutputQueue OutputQueue;
  static constexpr size_t OutputBulk = O::OutputBulkSize;

  // AsyncType
  static constexpr tasks::AsyncType Async = A;

  OExecutor() = default;
  OExecutor(const OExecutor& o) : calc(o.calc) {}

  OExecutor(OExecutor&&) = default;

  template <typename... Args,
            typename std::enable_if<std::is_constructible<Calc, Args...>{},
                                    bool>::type = true>
  OExecutor(Args&&... args) : calc(std::forward<Args>(args)...) {}

  TaskStatus execute(InputQueue* input_q, OutputQueue* output_q) {
    // Send function
    std::function<bool(std::optional<Output>&)> defaultSend;

    // Try send as much as possible
    auto trySend = [output_q](std::optional<Output>& obuff) {
      if (obuff.has_value()) {
        auto r = output_q->getQueue().try_emplace(std::move(obuff.value()));
        if (r) {
          obuff.reset();
        }
        return r;
      }
      return true;
    };

    // Block as long as not everything is sent
    auto blockSend = [output_q](std::optional<Output>& obuff) {
      if (obuff.has_value()) {
        output_q->getQueue().emplace(std::move(obuff.value()));
        obuff.reset();
      }
      return true;
    };

    if (Async == tasks::AsyncType::Synchronous) {
      // Synchronous may not block on output, or the whole pipeline will block
      defaultSend = blockSend;
    } else {
      defaultSend = trySend;
    }

    if (obuff.has_value()) {
      auto starved = !defaultSend(obuff);
      if (starved) {
        return TaskStatus::STARVED;
      }
      obuff.reset();
    }

    // Output Buffer
    while (!calc.finished()) {
      timer.start();
      calc.fillBuffer(obuff, blockSend);
      timer.stop();
      if (obuff.has_value()) {
        auto starved = !defaultSend(obuff);
        if (starved) {
          return TaskStatus::STARVED;
        }
        obuff.reset();
      }
    }
    return TaskStatus::FINISHED;
  }

  auto getData() const { return getData(calc); }

  auto toString() const { return toString(calc); }

  auto getTimer() const { return timer; }

 private:
  Calc calc;
  RecurringTimer timer;

  std::optional<Output> obuff;

  auto getData(const HasGetData auto& c) const { return c.getData(); }
  auto getData(const HasNoGetData auto& c) const { return 0; }

  auto toString(const HasToString auto& c) const { return c.toString(); }
  static auto constexpr toString(const HasNoToString auto& c) { return ""; }
};

template <class Calc, tasks::AsyncType A>
class IOExecutor {
 public:
  // Input
  typedef typename Calc::I I;
  typedef typename I::Input Input;
  typedef typename I::InputQueue InputQueue;
  static constexpr size_t InputBulk = I::InputBulkSize;

  // Output
  typedef typename Calc::O O;
  typedef typename O::Output Output;
  typedef typename O::OutputQueue OutputQueue;
  static constexpr size_t OutputBulk = O::OutputBulkSize;

  // AsyncType
  static constexpr tasks::AsyncType Async = A;

  IOExecutor() = default;
  IOExecutor(const IOExecutor& o) : calc(o.calc) {}

  IOExecutor(IOExecutor&&) = default;

  template <typename... Args,
            typename std::enable_if<std::is_constructible<Calc, Args...>{},
                                    bool>::type = true>
  IOExecutor(Args&&... args) : calc(std::forward<Args>(args)...) {}

  TaskStatus execute(InputQueue* input_q, OutputQueue* output_q) {
    // Send function
    std::function<bool(std::optional<Output>&)> defaultSend;

    // Try send as much as possible
    auto trySend = [output_q](std::optional<Output>& obuff) {
      if (obuff.has_value()) {
        auto r = output_q->getQueue().try_emplace(std::move(obuff.value()));
        if (r) {
          obuff.reset();
        }
        return r;
      }
      return true;
    };

    // Block as long as not everything is sent
    auto blockSend = [output_q](std::optional<Output>& obuff) {
      if (obuff.has_value()) {
        output_q->getQueue().emplace(std::move(obuff.value()));
        obuff.reset();
      }
      return true;
    };

    if (Async == tasks::AsyncType::Synchronous) {
      // Synchronous may not block on output, or the whole pipeline will block
      defaultSend = blockSend;
    } else {
      defaultSend = trySend;
    }

    if (obuff.has_value()) {
      auto starved = !defaultSend(obuff);
      if (starved) {
        return TaskStatus::STARVED;
      }
      obuff.reset();
    }


    size_t iterations = 0;
    while (!input_q->isFinished()) {
      // Get Input streams
      size_t tries = 0;
      while (!ibuff.has_value()) {
        tries++;

        // Normal consumer token retrieve
        Input i;
        bool success = input_q->getQueue().try_pop(i);
        if (success) {
          ibuff = std::move(i);
        } else {
          if (tries >= 50) {
            return TaskStatus::IDLE;
          }
          continue;
        }
      }

      while (ibuff.has_value()) {
        timer.start();
        // Calcs are expected to return buffer by default.
        // If intermediate results have to be sent, use blocking send
        calc.fillBuffer(ibuff, obuff, blockSend);
        timer.stop();
        if (obuff.has_value()) {
          auto starved = !defaultSend(obuff);
          if (starved) {
            return TaskStatus::STARVED;
          }
        }
      }
      iterations++;
      if (iterations > 2) return TaskStatus::YIELD;
    }

    timer.start();
    calc.finalize(obuff, defaultSend);
    timer.stop();
    if (obuff.has_value()) blockSend(obuff);
    return TaskStatus::FINISHED;
  }

  auto getData() const { return getData(calc); }

  auto toString() const { return toString(calc); }

  auto getTimer() const { return timer; }

 private:
  Calc calc;
  RecurringTimer timer;

  std::optional<Output> obuff;
  std::optional<Input> ibuff;

  constexpr size_t getFetchSize(const HasInputBulk auto& i) {
    return i.InputBulk;
  }
  constexpr size_t getFetchSize(const HasNoInputBulk auto& i) {
    return InputBulk;
  }

  auto getData(const HasGetData auto& c) const { return c.getData(); }
  auto getData(const HasNoGetData auto& c) const { return 0; }

  auto toString(const HasToString auto& c) const { return c.toString(); }
  static auto constexpr toString(const HasNoToString auto& c) { return ""; }
};

template <class Calc, tasks::AsyncType A>
class IExecutor {
 public:
  // Input
  typedef typename Calc::I I;
  typedef typename I::Input Input;
  typedef typename I::InputQueue InputQueue;
  static constexpr size_t InputBulk = I::InputBulkSize;

  // NoOpt Output
  typedef queue::NoOptOutputQueue O;
  typedef typename O::Output Output;
  typedef typename O::OutputQueue OutputQueue;
  static constexpr size_t OutputBulk = O::OutputBulkSize;

  // AsyncType
  static constexpr tasks::AsyncType Async = A;

  IExecutor() = default;
  IExecutor(const IExecutor& o) : calc(o.calc) {}
  IExecutor(IExecutor&&) = default;

  template <typename... Args,
            typename std::enable_if<std::is_constructible<Calc, Args...>{},
                                    bool>::type = true>
  IExecutor(Args&&... args) : calc(std::forward<Args>(args)...) {}

  TaskStatus execute(InputQueue* input_q, OutputQueue* output_q) {
    // Input Buffer
    std::optional<Input> ibuff;

    while (!input_q->isFinished()) {
      Input i;
      bool success = input_q->getQueue().try_pop(i);
      if (success) {
        ibuff = std::move(i);
      } else {
        return TaskStatus::IDLE;
      }

      timer.start();
      if (ibuff.has_value()) {
        calc.emptyBuffer(ibuff);
      }
      timer.stop();
    }
    timer.start();
    calc.finalize();
    timer.stop();
    return TaskStatus::FINISHED;
  }

  auto getData() const { return getData(calc); }

  auto toString() const { return toString(calc); }

  auto getTimer() const { return timer; }

 private:
  Calc calc;
  RecurringTimer timer;

  constexpr size_t getFetchSize(const HasInputBulk auto& i) {
    return i.InputBulk;
  }
  constexpr size_t getFetchSize(const HasNoInputBulk auto& i) {
    return InputBulk;
  }

  auto getData(const HasGetData auto& c) const { return c.getData(); }
  auto getData(const HasNoGetData auto& c) const { return 0; }

  auto toString(const HasToString auto& c) const { return c.toString(); }
  static auto constexpr toString(const HasNoToString auto& c) { return ""; }
};

}  // namespace joda::queryexecution::pipeline::tasks

#endif  // JODA_PIPELINE_EXECUTOR_TEMPLATE_H