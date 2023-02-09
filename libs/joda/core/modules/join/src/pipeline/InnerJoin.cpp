#include <joda/join/pipeline/InnerJoin.h>

void joda::queryexecution::pipeline::tasks::join::InnerJoinExec::emptyBuffer(
    std::optional<Input>& buff) {
  if (buff.has_value()) {
    auto cont = std::move(buff.value());
    buff.reset();
    if (cont == nullptr) {
      return;
    }
    // Join container
    joiner->addInner(cont);
  }
}

void joda::queryexecution::pipeline::tasks::join::InnerJoinExec::finalize() {
  joiner->setInnerFinished();
}

joda::queryexecution::pipeline::tasks::join::InnerJoinExec::InnerJoinExec(
    std::shared_ptr<joda::join::ContainerJoinExecutor> joiner)
    : joiner(std::move(joiner)) {}

std::string joda::queryexecution::pipeline::tasks::join::InnerJoinExec::toString() const{
  return joiner->toString();
}