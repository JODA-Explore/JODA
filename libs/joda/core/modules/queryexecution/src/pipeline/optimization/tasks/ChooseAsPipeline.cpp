#include "ChooseAsPipeline.h"

#include <joda/query/Query.h>

namespace joda::queryexecution::pipeline::tasks::optimization {

void ChooseAsExec::fillBuffer(
    std::optional<I::Input>& ibuff, std::optional<O::Output>& obuff,
    std::function<void(std::optional<O::Output>&)> sendPartial) {
  if(ibuff.has_value()) {
    auto cont = std::move(ibuff.value());
    ibuff.reset();
    if (cont == nullptr) {
      return;
    }

    auto o = handleOne(cont);  // Filter and transform documents
    if (o.has_value()) {
      DCHECK(*o != nullptr);
      obuff = std::move(*o);
      return;
    }

    
  }
}

std::optional<ChooseAsExec::O::Output> ChooseAsExec::handleOne(
    I::Input& input) {
  auto intermediate = choose.handleOne(input);
  input = nullptr; // (potentially) deallocate input
  if (!intermediate.has_value()) {
    return std::nullopt;
  }
  return as.handleOne(intermediate.value());
}

void ChooseAsExec::finalize(std::optional<O::Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial) {}

ChooseAsExec::ChooseAsExec(
    const std::vector<std::unique_ptr<IQueryExecutor>>& execs,
    const std::shared_ptr<const query::Query>& query)
    : choose(execs, query), as(query) {}

ChooseAsExec::ChooseAsExec(const ChooseAsExec& other)
    : choose(other.choose), as(other.as) {}

std::string ChooseAsExec::toString() const {
  return "CHOOSE: " + choose.toString() + " AS: " + as.toString();
}

}  // namespace joda::queryexecution::pipeline::tasks::optimization