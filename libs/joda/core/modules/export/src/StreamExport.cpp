

#include <utility>
#include <iostream>

#include "../include/joda/export/StreamExport.h"

//  _____ _            _ _
// |  __ (_)          | (_)
// | |__) | _ __   ___| |_ _ __   ___
// |  ___/ | '_ \ / _ \ | | '_ \ / _ \.
// | |   | | |_) |  __/ | | | | |  __/
// |_|   |_| .__/ \___|_|_|_| |_|\___|
//         | |
//         |_|

void joda::queryexecution::pipeline::tasks::store::WriteOutStreamExec::emptyBuffer(
    std::optional<Input>& buff) {
  
    std::move(buff->begin(), buff->end(),
              std::ostream_iterator<std::string>(std::cout, "\n"));

  buff.reset();
}

void joda::queryexecution::pipeline::tasks::store::WriteOutStreamExec::finalize() {}

joda::queryexecution::pipeline::tasks::store::WriteOutStreamExec::WriteOutStreamExec(){}

//  _____ ______                       _
// |_   _|  ____|                     | |
//   | | | |__  __  ___ __   ___  _ __| |_ ___ _ __
//   | | |  __| \ \/ / '_ \ / _ \| '__| __/ _ \ '__|
//  _| |_| |____ >  <| |_) | (_) | |  | ||  __/ |
// |_____|______/_/\_\ .__/ \___/|_|   \__\___|_|
//                   | |
//                   |_|


const std::string StreamExport::getTimerName() { return "Stream Export"; }

StreamExport::StreamExport() {
}


const std::string StreamExport::toString() {
  return "Export to stream" ;
}

const std::string StreamExport::toQueryString() {
  return "STORE IN STREAM";
}

 StreamExport::PipelineTaskPtr StreamExport::getTask() const {
  return std::make_unique<joda::queryexecution::pipeline::tasks::store::WriteOutStreamTask>();
 }