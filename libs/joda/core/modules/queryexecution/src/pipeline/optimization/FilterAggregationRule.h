#ifndef JODA_PIPELINE_FILTER_AGGREGATION_OPTIMIZATION_RULE_H
#define JODA_PIPELINE_FILTER_AGGREGATION_OPTIMIZATION_RULE_H

#include <list>
#include <vector>

#include "../tasks/agg/AggPipeline.h"
#include "../tasks/agg/FilterAggPipeline.h"
#include "../tasks/as/AsPipeline.h"
#include "OptimizationRule.h"

namespace joda::queryexecution::pipeline::optimization {

/**
 * @brief Combines a StorageReceiver and StorageSender into a StorageBuffer to
 * combine two queries into one
 */
class FilterAggregationRule : public OptimizationRule {
 public:
  /**
   * Creates a new Optimization rule which replace the given set of tasks with a
   * optimized subset.
   * @param tasks The tasks to optimize.
   */
  FilterAggregationRule()
      : OptimizationRule({tasks::ID<tasks::as::AsPipeline>::NAME,
                          tasks::ID<tasks::agg::AggPipeline>::NAME}) {}
  virtual ~FilterAggregationRule() = default;

  virtual std::string getName() const override {
    return "FilterAggregationRule";
  }

  virtual bool validateOptimizationTarget(const TaskListRange& range) override;

 protected:
  std::shared_ptr<const joda::query::Query> query;

  virtual TaskList getReplacement(const TaskListRange& range) const override;

  virtual void reset() override;
};

}  // namespace joda::queryexecution::pipeline::optimization

#endif  // JODA_PIPELINE_FILTER_AGGREGATION_OPTIMIZATION_RULE_H