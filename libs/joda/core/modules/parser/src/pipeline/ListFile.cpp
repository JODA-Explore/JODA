#include <joda/parser/pipeline/ListFile.h>

template<bool Lineseparated>
void joda::queryexecution::pipeline::tasks::load::ListExec<Lineseparated>::fillBuffer(
    std::optional<Output>& buff, std::function<void(std::optional<Output>&)> sendPartial) {
  try {
    Output path = std::make_pair(filename, sample);
    if (std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename)) {
      buff = std::move(path);
    } else {
      LOG(ERROR) << "File does not exist: '" << filename;
    }
  } catch (std::exception& e) {
    LOG(ERROR) << "Invalid file name '" << filename << "': " << e.what();
  }
  filename.clear();
}

template<bool Lineseparated>
bool joda::queryexecution::pipeline::tasks::load::ListExec<Lineseparated>::finished() const {
  return filename.empty();
}

template<bool Lineseparated>
joda::queryexecution::pipeline::tasks::load::ListExec<Lineseparated>::ListExec(
    const std::string& filename, const double sample)
    : filename(filename), sample(sample) {}

template<bool Lineseparated>
std::string joda::queryexecution::pipeline::tasks::load::ListExec<Lineseparated>::toString() const{
  if(sample == 1.0){
    return filename;
  }
  return filename + " sampled " + std::to_string(sample);
  
}

template class joda::queryexecution::pipeline::tasks::load::ListExec<true>;
template class joda::queryexecution::pipeline::tasks::load::ListExec<false>;