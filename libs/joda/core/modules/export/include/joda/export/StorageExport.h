//
// Created by Nico on 20/03/2019.
//

#ifndef JODA_STORAGEEXPORT_H
#define JODA_STORAGEEXPORT_H

#include <joda/storage/JSONStorage.h>
#include "IExportDestination.h"

class StorageExport : public IExportDestination {
 public:
  StorageExport(std::shared_ptr<JSONStorage> store);
  unsigned long getTemporaryResultID() const;
  const std::string toString() override;
  const std::string toQueryString() override;
  std::string getStorageName() const;
  const std::shared_ptr<JSONStorage> &getStore() const;
  virtual PipelineTaskPtr getTask() const override;

 protected:
  const std::string getTimerName() override;

 private:
  std::shared_ptr<JSONStorage> store;
};

#endif  // JODA_STORAGEEXPORT_H
