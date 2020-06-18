//
// Created by Nico on 21/05/2019.
//

#ifndef JODA_GROUPAGGREGATOR_H
#define JODA_GROUPAGGREGATOR_H

#include "IAggregator.h"
#include <unordered_map>

namespace joda::query {
class GroupAggregator : public IAggregator {
 public:
  GroupAggregator(const std::string &toPointer,
                  std::unique_ptr<IValueProvider> &&groupBy,
                  std::unique_ptr<IAggregator> &&agg, std::string groupAs = "");
  ~GroupAggregator() override = default;

  void merge(IAggregator *other) override;

  std::unique_ptr<IAggregator> duplicate() const override;

  static constexpr auto getName_() { return "GROUP"; }

  const std::string getName() const override;
  std::string toString() const override;

  RJValue terminate(RJMemoryPoolAlloc &alloc) override;

  void accumulate(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) override;

  void setGroupAs(const std::string &groupAs);
 protected:
  const std::unique_ptr<IAggregator> protoAgg;
  const std::unique_ptr<IValueProvider> groupBy;

  /*
   * Groups
   */
  typedef std::unordered_map<std::string, std::unique_ptr<IAggregator>> StringAggregators;
  StringAggregators stringGroups;
  typedef std::unordered_map<double, std::unique_ptr<IAggregator>> NumAggregators;
  NumAggregators numGroups;
  std::unique_ptr<IAggregator> trueAgg = nullptr;
  std::unique_ptr<IAggregator> falseAgg = nullptr;

  const std::string getGroupName() const;
  const std::string getValueName() const;
  std::string groupAs;
};
}

#endif //JODA_GROUPAGGREGATOR_H
