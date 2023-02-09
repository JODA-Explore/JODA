

#include "../include/joda/export/DirectoryExport.h"

#include <fstream>
#include <utility>

//  _____ _            _ _
// |  __ (_)          | (_)
// | |__) | _ __   ___| |_ _ __   ___
// |  ___/ | '_ \ / _ \ | | '_ \ / _ \.
// | |   | | |_) |  __/ | | | | |  __/
// |_|   |_| .__/ \___|_|_|_| |_|\___|
//         | |
//         |_|

void joda::queryexecution::pipeline::tasks::store::WriteFilesExec::emptyBuffer(
    std::optional<Input>& buff) {
      auto filename = dirname+"/"+std::to_string(num)+".json";
  std::ofstream newfile;
  newfile.open(filename, std::ofstream::out | std::ofstream::app);

  if (newfile) {

    std::move(buff->begin(), buff->end(),
              std::ostream_iterator<std::string>(newfile, "\n"));
  } else {
    LOG(ERROR) << "Could not open file " << filename
               << ". Error: " << strerror(errno);
  }
  buff.reset();
}

void joda::queryexecution::pipeline::tasks::store::WriteFilesExec::finalize() {}

joda::queryexecution::pipeline::tasks::store::WriteFilesExec::WriteFilesExec(
    const std::string& dirname)
    : dirname(dirname) {}

joda::queryexecution::pipeline::tasks::store::WriteFilesExec::WriteFilesExec(const 
    WriteFilesExec& other)
    : dirname(other.dirname) {
  num = other.num;
  other.num++;
}

//  _____ ______                       _
// |_   _|  ____|                     | |
//   | | | |__  __  ___ __   ___  _ __| |_ ___ _ __
//   | | |  __| \ \/ / '_ \ / _ \| '__| __/ _ \ '__|
//  _| |_| |____ >  <| |_) | (_) | |  | ||  __/ |
// |_____|______/_/\_\ .__/ \___/|_|   \__\___|_|
//                   | |
//                   |_|

const std::string DirectoryExport::getTimerName() { return "Directory Export"; }

DirectoryExport::DirectoryExport(std::string dirname)
    : dirname(std::move(dirname)) {
  // TODO: Check if dir exists
}

const std::string DirectoryExport::toString() {
  return "Export into Directory: " + dirname;
}

const std::string DirectoryExport::toQueryString() {
  return "STORE AS FILES \"" + dirname + "\"";
}

 DirectoryExport::PipelineTaskPtr DirectoryExport::getTask() const  {
  return std::make_unique<joda::queryexecution::pipeline::tasks::store::WriteFilesTask>(dirname);
};
