#ifndef JODA_HISTOGRAMAGGREGATOR_H
#define JODA_HISTOGRAMAGGREGATOR_H

#include <boost/histogram.hpp>

#include "IAggregator.h"

namespace joda::query {
class HistogramAggregator : public IAggregator {
 public:
  HistogramAggregator(const std::string &toPointer,
                      std::vector<std::unique_ptr<IValueProvider>> &&params);
  void merge(IAggregator *other) override;
  RJValue terminate(RJMemoryPoolAlloc &alloc) override;
  std::unique_ptr<IAggregator> duplicate() const override;
  void accumulate(const RapidJsonDocument &json,
                  RJMemoryPoolAlloc &alloc) override;
  const std::string getName() const override;

  static constexpr auto getName_() { return "HISTOGRAM"; }

 protected:
  boost::histogram::histogram<std::tuple<boost::histogram::axis::regular<>>>
      hist;
};
}  // namespace joda::query

#endif  // JODA_HISTOGRAMAGGREGATOR_H
