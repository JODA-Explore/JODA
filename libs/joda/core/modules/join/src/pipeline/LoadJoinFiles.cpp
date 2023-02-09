#include <joda/join/pipeline/LoadJoinFiles.h>

void joda::queryexecution::pipeline::tasks::join::LoadJoinFilesExec::fillBuffer(
    std::optional<Output>& obuff,
    std::function<void(std::optional<Output>&)> sendPartial) {
  DCHECK(manager != nullptr);

  auto files = manager->getJoinFiles();
  while(!files.empty()){
    obuff = std::move(files.back());
    files.pop_back();
    sendPartial(obuff);
    
  }
  
  isfinished = true;
}


bool joda::queryexecution::pipeline::tasks::join::LoadJoinFilesExec::finished()
    const {
  return isfinished;
}

joda::queryexecution::pipeline::tasks::join::LoadJoinFilesExec::LoadJoinFilesExec(
    const std::shared_ptr<FileJoinManager>& manager)
    : manager(manager) {}