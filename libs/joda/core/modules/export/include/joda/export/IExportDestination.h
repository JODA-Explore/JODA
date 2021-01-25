//
// Created by Nico on 20/03/2019.
//

#ifndef JODA_IEXPORTDESTINATION_H
#define JODA_IEXPORTDESTINATION_H

#include <joda/container/ContainerFlags.h>
#include <joda/misc/RecurringTimer.h>

/**
 * This Interface represents a method to export the documents contained in JODA
 */
class IExportDestination {
 public:
  virtual ~IExportDestination() = default;

  /**
   * Consumes (and times) a queue of containers.
   * Normally this queue is filled with the results of a query.
   * @param queue The queue to consume from
   */
  virtual void consume(JsonContainerQueue::queue_t &queue) {
    timer.start();
    this->consumeContainer(queue);
    timer.stop();
  };

  /**
   * Returns a pair consisting of the name of the Export Destination and a time (in seconds) it required to consume the queues.
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

 protected:
  virtual void consumeContainer(JsonContainerQueue::queue_t &queue) {
    std::unique_ptr<JSONContainer> cont;
    while (!queue.isFinished()) {
      queue.retrieve(cont);
      if (cont == nullptr) {
        LOG(WARNING) << "Received nullptr as container.";
        continue;
      }
      this->exportContainer(std::move(cont));
    }
  };
  virtual void exportContainer(std::unique_ptr<JSONContainer> && cont) = 0;
  virtual const std::string getTimerName() = 0;
  RecurringTimer timer;
};

#endif //JODA_IEXPORTDESTINATION_H
