#include <joda/document/InStreamOrigin.h>
#include <joda/parser/pipeline/InStream.h>

#include <iostream>

void joda::queryexecution::pipeline::tasks::load::InStreamExec::fillBuffer(
    std::optional<Output>& buff,
    std::function<void(std::optional<Output>&)> sendPartial) {
  auto s = docparsing::StreamOrigin{std::make_unique<InStreamOrigin>(),
                                    std::unique_ptr<std::istream>(&std::cin)};

  buff = std::make_pair(std::move(s), sample);

  sent = true;
}

bool joda::queryexecution::pipeline::tasks::load::InStreamExec::finished()
    const {
  return sent;
}

joda::queryexecution::pipeline::tasks::load::InStreamExec::InStreamExec(
    const double sample)
    : sample(sample) {}