#include <joda/parser/pipeline/LineSeparatedStreamReader.h>

void joda::queryexecution::pipeline::tasks::load::
    LineSeparatedStreamReaderExec::fillBuffer(std::optional<Input>& ibuff,
                                                   std::optional<Output>& obuff, std::function<void(std::optional<Output>&)> sendPartial) {
  if(ibuff.has_value()) {
    auto in = std::move(ibuff.value());
    ibuff.reset();
    auto& origin = in.first;
    auto sample = in.second; 

    auto& in_origin = origin.streamOrigin;
    auto& stream = origin.stream;
    if (stream == nullptr || stream->bad()) {
      DLOG(WARNING) << "Got null or bad stream";
      return;
    }
    if (in_origin == nullptr) {
      DLOG(WARNING) << "Got null origin";
      return;
    }

    if(!obuff.has_value()){
      obuff = std::make_optional<Output>(std::vector<joda::docparsing::StringOrigin>());
      obuff->reserve(OutBulk);
    }

    index = 0;
    long start = 0;
    long end = 0;
    std::string line;
    while (!stream->eof()) {
      start = stream->tellg();
      std::getline(*stream, line, '\n');
      end = stream->tellg();
      if (line.empty()) return;
      if (sample < 1.0 && (static_cast<double>(rand() % 100)) / 100.0 >=
                                      sample) {  // Sampling
        return;
      }
      in_origin->setIndex(index);
      in_origin->setStart(start);
      in_origin->setEnd(end);
      auto o = docparsing::StringOrigin{in_origin->cloneSpecific(), std::move(line)};
      obuff->emplace_back(std::move(o));
      index++;
      if (obuff->size() >= OutBulk) {
        sendPartial(obuff);
        DCHECK(!obuff.has_value());
      }
    }
    
  }
}


joda::queryexecution::pipeline::tasks::load::LineSeparatedStreamReaderExec::
    LineSeparatedStreamReaderExec() {}

joda::queryexecution::pipeline::tasks::load::LineSeparatedStreamReaderExec::LineSeparatedStreamReaderExec(const LineSeparatedStreamReaderExec& o){

}