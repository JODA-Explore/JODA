#include "AggPipeline.h"

#include <joda/query/Query.h>

namespace joda::queryexecution::pipeline::tasks::agg {

void AggExec::fillBuffer(
    std::optional<Input>& ibuff, std::optional<Output>& obuff,
    std::function<void(std::optional<Output>&)> sendPartial) {
  if(ibuff.has_value()) {
    auto cont = std::move(ibuff.value());
    ibuff.reset();

    handleOne(cont);
  }
}

void AggExec::handleOne(I::Input& input) {
  if (input->isView()) {
    input->materializeAttributesIfRequired(aggAttributes);
  }

  input->forAll([this](const RapidJsonDocument& d) {
    for (auto& j : aggregators) {
      j->accumulate(d, aggAlloc);
    }
  });
}

void AggExec::finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial) {
  for (auto&& agg : aggregators) {
    obuff = std::move(agg);
    sendPartial(obuff);
  }
}

AggExec::AggExec(const std::shared_ptr<const query::Query>& query)
    : query(query), aggAttributes(query->getAGGAttributes()), aggregators() {
  for (const auto& agg : query->getAggregators()) {
    aggregators.emplace_back(agg->duplicate());
  }
}

AggExec::AggExec(const AggExec& other)
    : query(other.query),
      aggAttributes(query->getAGGAttributes()),
      aggregators() {
  for (const auto& agg : query->getAggregators()) {
    aggregators.emplace_back(agg->duplicate());
  }
}

}  // namespace joda::queryexecution::pipeline::tasks::agg