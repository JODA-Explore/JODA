#include "ChooseAsAggPipeline.h"

#include <joda/query/Query.h>

namespace joda::queryexecution::pipeline::tasks::optimization {

void ChooseAsAggExec::fillBuffer(
    std::optional<I::Input>& ibuff, std::optional<O::Output>& obuff,
    std::function<void(std::optional<O::Output>&)> sendPartial) {
  if(ibuff.has_value()) {
    auto cont = std::move(ibuff.value());
    ibuff.reset();
    if (cont == nullptr) {
      return;
    }

    handleOne(cont);  // Filter, transform, and aggregate documents
  }
}

void ChooseAsAggExec::handleOne(
    I::Input& input) {
  auto choose_result = choose.handleOne(input);
  input = nullptr; // (potentially) deallocate input
  if (!choose_result.has_value()) {
    return;
  }
  auto as_result = as.handleOne(choose_result.value());
  choose_result.reset(); // (potentially) deallocate choose_result
  if (!as_result.has_value()) {
    return;
  }
  agg.handleOne(as_result.value());
}

void ChooseAsAggExec::finalize(std::optional<O::Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial) {
  agg.finalize(obuff, sendPartial);
}

ChooseAsAggExec::ChooseAsAggExec(
    const std::vector<std::unique_ptr<IQueryExecutor>>& execs,
    const std::shared_ptr<const query::Query>& query)
    : choose(execs, query), as(query), agg(query) {}

ChooseAsAggExec::ChooseAsAggExec(const ChooseAsAggExec& other)
    : choose(other.choose), as(other.as), agg(other.agg) {}

std::string ChooseAsAggExec::toString() const {
  //TODO add AGG string
  return "CHOOSE: " + choose.toString() + " AS: " + as.toString();
}

}  // namespace joda::queryexecution::pipeline::tasks::optimization