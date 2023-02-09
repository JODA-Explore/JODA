//
// Created by Nico on 20/03/2019.
//

#ifndef JODA_IEXPORTDESTINATION_H
#define JODA_IEXPORTDESTINATION_H

#include <joda/container/ContainerFlags.h>
#include <joda/misc/RecurringTimer.h>
#include <joda/pipelineatomics/PipelineTask.h>

/**
 * This Interface represents a method to export the documents contained in JODA
 */
class IExportDestination {
  protected:
    typedef joda::queryexecution::pipeline::tasks::PipelineTaskPtr PipelineTaskPtr;
 public:
  virtual ~IExportDestination() = default;

  /**
   * Returns a pair consisting of the name of the Export Destination and a time
   * (in seconds) it required to consume the queues.
   * @return The timing of the execution.
   */
  std::pair<std::string, double> getTimer() {
    return {getTimerName(), timer.durationSeconds()};
  };

  /**
   * Returns a (human readable) string describing the ExportDestination.
   * A.toString() == B.toString() => A == B
   * @return
   */
  virtual const std::string toString() = 0;

  /**
   * Returns a string as this ExportDestination would be represented in a query
   * @return
   */
  virtual const std::string toQueryString() = 0;

  /**
   * Returns the exporting task
   * @return the task to be executed to export the documents
   */
  virtual PipelineTaskPtr getTask() const = 0;

 protected:

  virtual const std::string getTimerName() = 0;
  RecurringTimer timer;
};

#endif  // JODA_IEXPORTDESTINATION_H
