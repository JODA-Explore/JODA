#include <joda/parser/pipeline/ListDir.h>

#include <boost/algorithm/string.hpp>

template <bool Lineseparated>
void joda::queryexecution::pipeline::tasks::load::ListDirExec<Lineseparated>::
    fillBuffer(std::optional<Output>& buff,
               std::function<void(std::optional<Output>&)> sendPartial) {
  try {
    std::filesystem::path dirpath = dirname;
    dirname.clear();
    if (!std::filesystem::exists(dirpath)) {
      LOG(ERROR) << "Dir does not exist: '" << dirpath;
      return;
    }
    if (!std::filesystem::is_directory(dirpath)) {
      LOG(ERROR) << "Path is not a directory: '" << dirpath;
      return;
    }
    // Iterate directory
    for (auto const& dir_entry : std::filesystem::directory_iterator{dirpath}) {
      // Is JSON file?
      if (std::filesystem::is_regular_file(dir_entry.path()) &&
          boost::iequals(dir_entry.path().extension().string(), ".json")) {
        buff = std::make_pair(dir_entry.path(), sample);
        sendPartial(buff);
      }
    }

  } catch (std::exception& e) {
    LOG(ERROR) << "Invalid directory name '" << dirname << "': " << e.what();
  }
}

template <bool Lineseparated>
bool joda::queryexecution::pipeline::tasks::load::ListDirExec<
    Lineseparated>::finished() const {
  return dirname.empty();
}

template <bool Lineseparated>
joda::queryexecution::pipeline::tasks::load::ListDirExec<
    Lineseparated>::ListDirExec(const std::string& dirname, const double sample)
    : dirname(dirname), sample(sample) {}

template <bool Lineseparated>
std::string joda::queryexecution::pipeline::tasks::load::ListDirExec<
    Lineseparated>::toString() const {
  if (sample == 1.0) {
    return dirname;
  }
  return dirname + " sampled " + std::to_string(sample);
}

template class joda::queryexecution::pipeline::tasks::load::ListDirExec<true>;
template class joda::queryexecution::pipeline::tasks::load::ListDirExec<false>;