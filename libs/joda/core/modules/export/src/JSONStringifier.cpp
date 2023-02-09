#include "../include/joda/export/JSONStringifier.h"

void joda::queryexecution::pipeline::tasks::store::JSONStringifierExec::
    fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff,
               std::function<void(std::optional<Output>&)> sendPartial) {
  size_t bulk_size = OutBulk;

  if (ibuff.has_value()) {
    auto cont = std::move(ibuff.value());
    ibuff.reset();
    if (cont == nullptr) return;

    DCHECK(!obuff.has_value());

    // Start at 0
    size_t start = 0;

    while (true) {
      // Stringify one bulk
      auto strings = cont->stringify(start, (start + bulk_size) - 1);
      // Move start position
      start += bulk_size;
      DCHECK(strings.size() <= bulk_size);

      // If all strings retrieved, break
      if (strings.size() < bulk_size) {
        obuff = std::move(strings);
        return;
      }
      // Otherwise, send strings
      obuff = std::move(strings);
      sendPartial(obuff);
      obuff.reset();
    }
  }
}

void joda::queryexecution::pipeline::tasks::store::JSONStringifierExec::
    finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial) {}

joda::queryexecution::pipeline::tasks::store::JSONStringifierExec::
    JSONStringifierExec() {}
