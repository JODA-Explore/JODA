//
// Created by Nico on 06/12/2018.
//

#ifndef JODA_ITHREADUSER_H
#define JODA_ITHREADUSER_H
#include <glob.h>

#define JODA_THREAD_SINGLE size_t(1)
#define JODA_THREAD_NONE size_t(0)
#define JODA_THREAD_MAX std::numeric_limits<std::size_t>::max()

class IThreadUser {
 public:
  explicit IThreadUser(size_t maxThreads) : maxThreads(maxThreads) {}

  virtual void forceThreads(size_t threads) = 0;
  virtual size_t getUsedThreads() const = 0;

  virtual size_t getMaxThreads() const {
    return maxThreads;
  }

  virtual void setMaxThreads(size_t maxThreads) {
    IThreadUser::maxThreads = maxThreads;
  }

  virtual size_t recommendedThreads() const = 0;
 protected:

  size_t maxThreads;

};

#endif //JODA_ITHREADUSER_H
