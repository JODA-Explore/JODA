#include <joda/join/JoinParser.h>
#include <joda/join/pipeline/LoadJoinFileParser.h>

void joda::queryexecution::pipeline::tasks::join::LoadJoinFileParserExec::
    fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff,
               std::function<void(std::optional<Output>&)> sendPartial) {
  DCHECK(manager != nullptr);

  JoinParser parser;
  auto cont = std::make_shared<JSONContainer>();
  if (ibuff.has_value()) {
    auto file = std::move(ibuff.value());
    ibuff.reset();
    parser.parse(*manager, file, *cont);
    if (!cont->hasSpace(0)) {
      cont->finalize();
      obuff = std::move(cont);
      sendPartial(obuff);

      cont = std::make_shared<JSONContainer>();
    }
  }
  if (cont->size() != 0) {
    cont->finalize();
    obuff = std::move(cont);
  }
}

void joda::queryexecution::pipeline::tasks::join::LoadJoinFileParserExec::
    finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial) {}

joda::queryexecution::pipeline::tasks::join::LoadJoinFileParserExec::
    LoadJoinFileParserExec(const std::shared_ptr<FileJoinManager>& manager)
    : manager(manager) {}

joda::queryexecution::pipeline::tasks::join::LoadJoinFileParserExec::
    ~LoadJoinFileParserExec() {}