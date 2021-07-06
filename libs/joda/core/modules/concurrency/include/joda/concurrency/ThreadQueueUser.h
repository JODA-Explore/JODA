//
// Created by Nico on 11/12/2018.
//

#ifndef JODA_THREADQUEUEUSER_H
#define JODA_THREADQUEUEUSER_H
#include "IThreadUser.h"

class ThreadQueueUser : public IThreadUser {
 public:
  size_t getQueueRead() {
    queueReadDelta = queueRead;
    return queueRead;
  }

  size_t getQueueWritten() {
    queueWrittenDelta = queueWritten;
    return queueWritten;
  }

  size_t getQueueReadDelta() {
    auto i = queueRead - queueReadDelta;
    queueReadDelta = queueRead;
    return i;
  }

  size_t getQueueWrittenDelta() {
    auto i = queueWritten - queueWrittenDelta;
    queueWrittenDelta = queueWritten;
    return i;
  }

  virtual void updateStatistics() = 0;

 protected:
  size_t queueRead{};
  size_t queueReadDelta{};
  size_t queueWritten{};
  size_t queueWrittenDelta{};

  virtual void addStatistics(size_t read, size_t written) {
    queueRead += read;
    queueWritten += written;
  }
};
#endif  // JODA_THREADQUEUEUSER_H
