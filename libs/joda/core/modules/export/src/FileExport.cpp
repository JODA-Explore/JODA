//
// Created by Nico on 20/03/2019.
//

#include "../include/joda/export/FileExport.h"

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

void joda::queryexecution::pipeline::tasks::store::WriteFileExec::emptyBuffer(
    std::optional<Input>& buff) {
  std::ofstream newfile;
  newfile.open(filename.c_str(), std::ofstream::out | std::ofstream::app);

  if (newfile) {
    std::move(buff->begin(), buff->end(),
              std::ostream_iterator<std::string>(newfile, "\n"));
  } else {
    LOG(ERROR) << "Could not open file " << filename
               << ". Error: " << strerror(errno);
  }
  buff.reset();
}

void joda::queryexecution::pipeline::tasks::store::WriteFileExec::finalize() {}

joda::queryexecution::pipeline::tasks::store::WriteFileExec::WriteFileExec(
    const std::string& filename)
    : filename(filename) {}

//  _____ ______                       _
// |_   _|  ____|                     | |
//   | | | |__  __  ___ __   ___  _ __| |_ ___ _ __
//   | | |  __| \ \/ / '_ \ / _ \| '__| __/ _ \ '__|
//  _| |_| |____ >  <| |_) | (_) | |  | ||  __/ |
// |_____|______/_/\_\ .__/ \___/|_|   \__\___|_|
//                   | |
//                   |_|

const std::string FileExport::getTimerName() { return "File Export"; }

FileExport::FileExport(std::string filename) : filename(std::move(filename)) {
  // TODO: test file/dir?
}

FileExport::PipelineTaskPtr FileExport::getTask() const{
  return std::make_unique<joda::queryexecution::pipeline::tasks::store::WriteFileTask>(filename);
};

const std::string FileExport::toString() {
  return "Export into file: " + filename;
}

const std::string FileExport::toQueryString() {
  return "STORE AS FILE \"" + filename + "\"";
}
