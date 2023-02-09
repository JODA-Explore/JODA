#ifndef JODA_PIPELINE_QUERY_COMBINER_OPTIMIZATION_RULE_H
#define JODA_PIPELINE_QUERY_COMBINER_OPTIMIZATION_RULE_H

#include "OptimizationRule.h"
#include "../tasks/storage/StorageBuffer.h"
#include "../tasks/storage/StorageReceiver.h"
#include "../tasks/storage/StorageSender.h"


#include <list>
#include <vector>

namespace joda::queryexecution::pipeline::optimization {


/**
 * @brief Combines a StorageReceiver and StorageSender into a StorageBuffer to combine two queries into one
 */
class QueryCombinationRule : public OptimizationRule {
 public:
  /**
   * Creates a new Optimization rule which replace the given set of tasks with a
   * optimized subset.
   * @param tasks The tasks to optimize.
   */
  QueryCombinationRule() : OptimizationRule({tasks::ID<tasks::storage::StorageReceiver>::NAME,tasks::ID<tasks::storage::StorageSender>::NAME}) {}
  virtual ~QueryCombinationRule() = default;

  virtual std::string getName() const override {
    return "QueryCombinationRule";
  }

  virtual bool validateOptimizationTarget(const TaskListRange& range) override;


 protected:
  std::string name = "";

  virtual TaskList getReplacement(const TaskListRange& range) const override;

  virtual void reset() override;

};



}  // namespace joda::queryexecution::pipeline::optimization

#endif  // JODA_PIPELINE_QUERY_COMBINER_OPTIMIZATION_RULE_H