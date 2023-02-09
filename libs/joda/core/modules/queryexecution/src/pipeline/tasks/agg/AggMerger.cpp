#include "AggMerger.h"

#include <joda/document/TemporaryOrigin.h>

void joda::queryexecution::pipeline::tasks::agg::AggMergerExec::fillBuffer(
    std::optional<Input>& ibuff, std::optional<Output>& obuff,
    std::function<void(std::optional<Output>&)> sendPartial) {
  if(ibuff.has_value()) {
    auto agg = std::move(ibuff.value());
    ibuff.reset();

    // Get String ID of aggregator
    auto aStr = agg->toString();
    // Search in AggMap
    auto it = aggregators.find(aStr);
    if (it == aggregators.end()) {
      // Initialize if not found before
      aggregators.try_emplace(aStr, std::move(agg));
    } else {
      // Merge if found
      it->second->merge(agg.get());
    }
  }
}

void joda::queryexecution::pipeline::tasks::agg::AggMergerExec::finalize(
    std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial) {
  // Create Container
  auto cont = std::make_shared<JSONContainer>();

  // Create Document
  auto alloc = cont->getAlloc();
  auto doc = std::make_unique<RJDocument>(alloc);
  doc->SetObject();

  // Finalize Aggregators
  for (auto&& a : aggregators) {
    auto val = a.second->terminate(*alloc);
    RJPointer p(a.second->getDestPointer().c_str());
    p.Set(*doc, val);
  }

  // Build container
  cont->insertDoc(std::move(doc), std::make_unique<TemporaryOrigin>());
  cont->finalize();

  // Send container
  obuff = std::move(cont);
}
