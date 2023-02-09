#include "FilterAggPipeline.h"

#include <joda/query/Query.h>
namespace joda::queryexecution::pipeline::tasks::agg {

void FilterAggExec::fillBuffer(
    std::optional<Input>& ibuff, std::optional<Output>& obuff,
    std::function<void(std::optional<Output>&)> sendPartial) {
  if(ibuff.has_value()) {
    auto fcont = std::move(ibuff.value());
    ibuff.reset();

    handleOne(fcont);
  }
}

void FilterAggExec::handleOne(I::Input& input) {
  // Get real container
  auto cont = input.getContainer();
  if (input.isConst()) {  // Check if True/False selection
    auto constRes = input.getConstFilter();
    if (!constRes) {  // If false, skip
      return;
    }

    // Else, materialize ...
    if (cont->isView()) {
      cont->materializeAttributesIfRequired(aggAttributes);
    }

    // ... and aggregate
    cont->forAll([this](const RapidJsonDocument& d) {
      for (auto& j : aggregators) {
        j->accumulate(d, aggAlloc);
      }
    });

  } else {  // If not const,
    // materialize ...
    if (cont->isView()) {
      cont->materializeAttributesIfRequired(aggAttributes);
    }
    auto index = input.getIndex();
    // ... and aggregate filtered
    cont->forAll(
        [this](const RapidJsonDocument& d, size_t i) {
          for (auto& j : aggregators) {
            j->accumulate(d, aggAlloc);
          }
        },
        *index);
  }
}

void FilterAggExec::finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial) {
  for (auto&& agg : aggregators) {
    obuff = std::move(agg);
    sendPartial(obuff);
  }
}

FilterAggExec::FilterAggExec(const std::shared_ptr<const query::Query>& query)
    : query(query), aggAttributes(query->getAGGAttributes()), aggregators() {
  for (const auto& agg : query->getAggregators()) {
    aggregators.emplace_back(agg->duplicate());
  }
}

FilterAggExec::FilterAggExec(const FilterAggExec& other)
    : query(other.query),
      aggAttributes(query->getAGGAttributes()),
      aggregators() {
  for (const auto& agg : query->getAggregators()) {
    aggregators.emplace_back(agg->duplicate());
  }
}

}  // namespace joda::queryexecution::pipeline::tasks::agg