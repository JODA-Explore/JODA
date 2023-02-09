#include "ChooseAggPipeline.h"

#include <joda/query/Query.h>

namespace joda::queryexecution::pipeline::tasks::optimization {

void ChooseAggExec::fillBuffer(
    std::optional<I::Input>& ibuff, std::optional<O::Output>& obuff,
    std::function<void(std::optional<O::Output>&)> sendPartial) {
  if(ibuff.has_value()) {
    auto cont = std::move(ibuff.value());
    ibuff.reset();
    if (cont == nullptr) {
      return;
    }

    handleOne(cont);  // Filter, transform, and aggregate documents
    return;
  }
}

void ChooseAggExec::handleOne(
    I::Input& input) {
  auto choose_result = choose.handleOne(input);
  input = nullptr; // (potentially) deallocate input
  if (!choose_result.has_value()) {
    return;
  }
  agg.handleOne(choose_result.value());
}

void ChooseAggExec::finalize(std::optional<O::Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial) {
  agg.finalize(obuff, sendPartial);
}

ChooseAggExec::ChooseAggExec(
    const std::vector<std::unique_ptr<IQueryExecutor>>& execs,
    const std::shared_ptr<const query::Query>& query)
    : choose(execs, query), agg(query) {}

ChooseAggExec::ChooseAggExec(const ChooseAggExec& other)
    : choose(other.choose), agg(other.agg) {}

std::string ChooseAggExec::toString() const {
  //TODO add AGG string
  return "CHOOSE: " + choose.toString();
}

}  // namespace joda::queryexecution::pipeline::tasks::optimization