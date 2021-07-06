#ifndef JODA_COUNTAGGREGATOR_H
#define JODA_COUNTAGGREGATOR_H

#include "IAggregator.h"

namespace joda::query {
class CountAggregator : public IAggregator {
 public:
  CountAggregator(const std::string &toPointer,
                  std::vector<std::unique_ptr<IValueProvider>> &&params);
  void merge(IAggregator *other) override;
  RJValue terminate(RJMemoryPoolAlloc &alloc) override;
  std::unique_ptr<IAggregator> duplicate() const override;
  void accumulate(const RapidJsonDocument &json,
                  RJMemoryPoolAlloc &alloc) override;
  const std::string getName() const override;

  static constexpr auto getName_() { return "COUNT"; }

 protected:
  ulong count = 0;
};
}  // namespace joda::query

#endif  // JODA_COUNTAGGREGATOR_H
