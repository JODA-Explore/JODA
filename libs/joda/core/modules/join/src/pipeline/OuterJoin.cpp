#include <joda/join/pipeline/OuterJoin.h>

void joda::queryexecution::pipeline::tasks::join::OuterJoinExec::fillBuffer(
    std::optional<I::Input>& ibuff, std::optional<O::Output>& obuff,
    std::function<void(std::optional<O::Output>&)> sendPartial) {
  if(ibuff.has_value()) {
    auto cont = std::move(ibuff.value());
    ibuff.reset();
    if (cont == nullptr) {
      return;
    }
    // Join container
    obuff = joiner->joinContainer(cont);
  }
}

void joda::queryexecution::pipeline::tasks::join::OuterJoinExec::finalize(
    std::optional<O::Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial) {}

joda::queryexecution::pipeline::tasks::join::OuterJoinExec::OuterJoinExec(
    std::shared_ptr<joda::join::ContainerJoinExecutor> joiner)
    : joiner(std::move(joiner)) {}

std::string joda::queryexecution::pipeline::tasks::join::OuterJoinExec::toString() const{
  return joiner->toString();
}