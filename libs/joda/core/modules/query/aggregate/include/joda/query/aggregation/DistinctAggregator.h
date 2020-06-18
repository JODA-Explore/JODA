//
// Created by Nico Schäfer on 11/14/17.
//

#ifndef JODA_DISTINCTAGGREGATOR_H
#define JODA_DISTINCTAGGREGATOR_H

#include <unordered_set>
#include "IAggregator.h"

namespace joda::query {
class DistinctAggregator : public IAggregator {
 public:
  DistinctAggregator(const std::string& toPointer, std::vector<std::unique_ptr<IValueProvider>>&& params,
                     bool stringEnabled = true,
                     bool boolEnabled = true,
                     bool numEnabled = true

  );
  void merge(IAggregator *other) override;
  RJValue terminate(RJMemoryPoolAlloc &alloc) override;
  std::unique_ptr<IAggregator> duplicate() const override;
  void accumulate(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) override;
  const std::string getName() const override;

  static constexpr auto getName_() { return "DISTINCT"; }
 protected:
  bool stringEnabled;
  bool boolEnabled;
  bool numEnabled;
  std::unordered_set<std::string> strSet = {};
  std::unordered_set<double> numSet= {};
  bool hasTrue = false;
  bool hasFalse = false;
};
}

#endif //JODA_DISTINCTAGGREGATOR_H
