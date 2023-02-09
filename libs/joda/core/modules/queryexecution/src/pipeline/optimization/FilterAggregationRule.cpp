#include "FilterAggregationRule.h"
namespace joda::queryexecution::pipeline::optimization {
bool FilterAggregationRule::validateOptimizationTarget(
    const TaskListRange& range) {
  auto* asTask = dynamic_cast<tasks::as::AsPipelineTask*>(range.first->get());
  auto* aggTask =
      dynamic_cast<tasks::agg::AggPipelineTask*>(range.second->get());

  if (asTask == nullptr || aggTask == nullptr) {
    // Wrong types
    return false;
  }

  // Check if AS is empty => Only a selection unpacking step
  if (asTask->getDesc().empty()) {
    query = asTask->getData();
    return true;
  }
  return false;
}

OptimizationRule::TaskList FilterAggregationRule::getReplacement(
    const TaskListRange& range) const {
  DCHECK(query != nullptr);
  auto ptr = std::make_unique<tasks::agg::FilterAggPipelineTask>(query);
  TaskList list;
  list.emplace_back(std::move(ptr));
  return list;
}

void FilterAggregationRule::reset() { query = nullptr; }

}  // namespace joda::queryexecution::pipeline::optimization