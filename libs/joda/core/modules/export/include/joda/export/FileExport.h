//
// Created by Nico on 20/03/2019.
//

#ifndef JODA_FILEEXPORT_H
#define JODA_FILEEXPORT_H

#include "IExportDestination.h"

class FileExport : public IExportDestination {
 public:
  FileExport(const std::string &filename);
  const std::string toString() override;
  const std::string toQueryString() override;
 protected:
  void exportContainer(std::unique_ptr<JSONContainer> &&cont) override;
  const std::string getTimerName() override;
 private:
  std::string filename;

};

#endif //JODA_FILEEXPORT_H
