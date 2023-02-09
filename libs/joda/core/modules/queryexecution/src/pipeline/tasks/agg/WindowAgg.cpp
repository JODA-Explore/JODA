#include "WindowAgg.h"

#include <joda/query/Query.h>

namespace joda::queryexecution::pipeline::tasks::agg {

void WindowAggExec::fillBuffer(
    std::optional<Input>& ibuff, std::optional<Output>& obuff,
    std::function<void(std::optional<Output>&)> sendPartial) {
  if(ibuff.has_value()) {
    auto cont = std::move(ibuff.value());
    ibuff.reset();

    if (cont->isView()) {
      cont->materializeAttributesIfRequired(aggAttributes);
    }

    cont->forAll([this, &obuff, &sendPartial](const RapidJsonDocument& d) {
      for (auto& j : currentAggregators) {
        j->accumulate(d, aggAlloc);
      }
      aggregated++;
      // Window full, send window result
      if (aggregated >= windowSize) {
        obuff = finalizeCurrentAggregators();
        sendPartial(obuff);
      }
    });
  }
}

void WindowAggExec::finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial) {
  if (aggregated > 0) {
    obuff = finalizeCurrentAggregators();
  }
}

WindowAggExec::WindowAggExec(const std::shared_ptr<const query::Query>& query,
                             uint64_t windowSize)
    : query(query),
      aggAttributes(query->getAGGAttributes()),
      windowSize(windowSize),
      originalAggregators() {
  for (const auto& agg : query->getAggregators()) {
    originalAggregators.emplace_back(agg->duplicate());
  }
  cloneAggregators();
}

WindowAggExec::WindowAggExec(const WindowAggExec& other)
    : query(other.query),
      aggAttributes(query->getAGGAttributes()),
      windowSize(other.windowSize),
      originalAggregators()

{
  for (const auto& agg : query->getAggregators()) {
    originalAggregators.emplace_back(agg->duplicate());
  }
  cloneAggregators();
}

void WindowAggExec::cloneAggregators() {
  currentAggregators.clear();
  currentAggregators.reserve(originalAggregators.size());
  for (const auto& agg : originalAggregators) {
    currentAggregators.emplace_back(agg->duplicate());
  }
}

std::shared_ptr<JSONContainer> WindowAggExec::finalizeCurrentAggregators() {
  // Create Container
  auto cont = std::make_shared<JSONContainer>();

  // Create Document
  auto alloc = cont->getAlloc();
  auto doc = std::make_unique<RJDocument>(alloc);
  doc->SetObject();

  // Finalize Aggregators
  for (auto&& a : currentAggregators) {
    auto val = a->terminate(*alloc);
    RJPointer p(a->getDestPointer().c_str());
    p.Set(*doc, val);
  }

  // Reset current aggregators
  cloneAggregators();

  // Reset aggregated counter
  aggregated = 0;

  // Build container
  cont->insertDoc(std::move(doc), std::make_unique<TemporaryOrigin>());
  cont->finalize();
  return cont;
}

}  // namespace joda::queryexecution::pipeline::tasks::agg