#include <joda/parser/pipeline/FileOpener.h>

template<bool Lineseparated>
void joda::queryexecution::pipeline::tasks::load::FileOpenerExec<Lineseparated>::
    fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff, std::function<void(std::optional<Output>&)> sendPartial) {
  if(ibuff.has_value()) {
    auto input = std::move(ibuff.value());
    auto& path = input.first;
    ibuff.reset();
    try {
      auto file = std::make_unique<std::ifstream>();
      file->open(path);
      if (file->is_open()) {
        auto fileorigin =
            std::make_unique<FileOrigin>(g_FileNameRepoInstance.addFile(path));
        auto s = docparsing::StreamOrigin{std::move(fileorigin), std::move(file)};

        obuff = std::make_pair(std::move(s),input.second);
      } else {
        LOG(ERROR) << "File does not exist: '" << path;
      }
    } catch (std::exception& e) {
      LOG(ERROR) << "Can't open file '" << path << "': " << e.what();
    }
  }
}

template<bool Lineseparated>
joda::queryexecution::pipeline::tasks::load::FileOpenerExec<Lineseparated>::FileOpenerExec() {}

template class joda::queryexecution::pipeline::tasks::load::FileOpenerExec<true>;
template class joda::queryexecution::pipeline::tasks::load::FileOpenerExec<false>;