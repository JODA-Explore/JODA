//
// Created by Nico on 20/03/2019.
//

#ifndef JODA_JOINEXPORT_H
#define JODA_JOINEXPORT_H

#include <joda/join/JoinManager.h>
#include "IExportDestination.h"

class JoinExport : public IExportDestination {
 public:
  JoinExport(std::shared_ptr<JoinManager> joinManager);
  const std::string toString() override;
  const std::string toQueryString() override;
  virtual PipelineTaskPtr getTask() const override;
  
 protected:
  const std::string getTimerName() override;

 private:
  std::shared_ptr<JoinManager> joinManager;
};

#endif  // JODA_JOINEXPORT_H
