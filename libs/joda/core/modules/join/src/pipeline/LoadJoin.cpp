#include <joda/join/pipeline/LoadJoin.h>

void joda::queryexecution::pipeline::tasks::join::LoadJoinExec::fillBuffer(
    std::optional<Output>& obuff,
    std::function<void(std::optional<Output>&)> sendPartial) {
  DCHECK(manager != nullptr);

  // Memory based join
  std::shared_ptr<JSONContainer> cont = manager->loadOneJoinCont();
  if(cont != nullptr) {
    obuff = std::move(cont);
    return;
  }
  isfinished = true;
}


bool joda::queryexecution::pipeline::tasks::join::LoadJoinExec::finished()
    const {
  return isfinished;
}

joda::queryexecution::pipeline::tasks::join::LoadJoinExec::LoadJoinExec(
    const std::shared_ptr<MemoryJoinManager>& manager)
    : manager(manager) {}