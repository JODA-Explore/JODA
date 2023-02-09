#include <joda/join/pipeline/StoreJoin.h>

void joda::queryexecution::pipeline::tasks::join::StoreJoinExec::emptyBuffer(
    std::optional<Input>& buff) {
  if (buff.has_value()) {
    auto cont = std::move(buff.value());
    buff.reset();
    if (cont == nullptr) {
      return;
    }
    // Join container
    manager->join(*cont);
  }
}

void joda::queryexecution::pipeline::tasks::join::StoreJoinExec::finalize() {}

joda::queryexecution::pipeline::tasks::join::StoreJoinExec::StoreJoinExec(
    std::shared_ptr<JoinManager> manager)
    : manager(std::move(manager)) {}
