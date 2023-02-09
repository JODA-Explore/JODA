#ifndef JODA_PIPELINE_TASKID_H
#define JODA_PIPELINE_TASKID_H

#include <cstdio>
namespace joda::queryexecution::pipeline::tasks {

/*
 *  _____ _____
 * |_   _|  __ \
 *   | | | |  | |___
 *   | | | |  | / __|
 *  _| |_| |__| \__ \
 * |_____|_____/|___/
 */
typedef std::string TaskID;

template <class T>
struct ID {
  // static constexpr size_t VALUE = 0;
};
}  // namespace joda::queryexecution::pipeline::tasks

/*
 * TCLASS:    Classname of Task
 */
#define JODA_REGISTER_PIPELINE_TASK(TCLASS, NAMESTR) \
  namespace joda::queryexecution::pipeline::tasks {  \
  template <>                                        \
  struct ID<TCLASS> {                                \
    static constexpr auto NAME = NAMESTR;            \
  };                                                 \
  }

#define JODA_CONCAT_HELPER(a, b) a ## b
#define JODA_CONCAT(a, b) JODA_CONCAT_HELPER(a, b)
#define JODA_TASK_SUFFIX Task
#define JODA_MAKE_TASKNAME(NAMESTR) JODA_CONCAT(NAMESTR, JODA_TASK_SUFFIX)

#define JODA_REGISTER_PIPELINE_TASK_IN_NS(TCLASS, NAMESTR, NS) \
  namespace joda::queryexecution::pipeline::tasks {            \
  template <>                                                  \
  struct ID<NS::TCLASS> {                                      \
    static constexpr auto NAME = NAMESTR;                      \
  };                                                           \
  }                                                            \
  namespace NS {                                               \
  typedef PipelineIOTask<TCLASS> JODA_MAKE_TASKNAME(TCLASS);   \
  }

#endif  // JODA_PIPELINE_TASKID_H