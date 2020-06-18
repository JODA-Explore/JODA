//
// Created by Nico on 18/03/2019.
//

#ifndef JODA_COLLECTAGGREGATOR_H
#define JODA_COLLECTAGGREGATOR_H
#include <joda/query/aggregation/IAggregator.h>

namespace joda::query {
class CollectAggregator : public IAggregator {
 public:
  CollectAggregator(const std::string& toPointer, std::vector<std::unique_ptr<IValueProvider>>&& params);

  void merge(IAggregator *other) override;
  RJValue terminate(RJMemoryPoolAlloc &alloc) override;
  std::unique_ptr<IAggregator> duplicate() const override;
  void accumulate(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) override;
  const std::string getName() const override;

  static constexpr auto getName_() { return "COLLECT"; }
 protected:
  std::unique_ptr< RJValue> list;
  std::unique_ptr<RJMemoryPoolAlloc> alloc;
};
}

#endif //JODA_COLLECTAGGREGATOR_H
