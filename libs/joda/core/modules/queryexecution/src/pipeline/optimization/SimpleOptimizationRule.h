#ifndef JODA_PIPELINE_SIMPLE_OPTIMIZATION_RULE_H
#define JODA_PIPELINE_SIMPLE_OPTIMIZATION_RULE_H

#include <joda/parser/pipeline/FileOpener.h>
#include <joda/parser/pipeline/LSFileMapper.h>
#include <joda/parser/pipeline/LineSeparatedStreamReader.h>
#include <joda/pipelineatomics/PipelineIOTask.h>
#include <joda/pipelineatomics/PipelineTask.h>

#include <list>
#include <vector>

#include "OptimizationRule.h"
#include "tasks/ChooseAggPipeline.h"
#include "tasks/ChooseAsAggPipeline.h"
#include "tasks/ChooseAsPipeline.h"

namespace joda::queryexecution::pipeline::optimization {

/**
 * @brief The SimpleOptimizationRule class, which represents a set of tasks that
 * may be replaced by a single task
 */
template <class R>
class SimpleOptimizationRule : public OptimizationRule {
 public:
  /**
   * Creates a new Optimization rule which replace the given set of tasks with a
   * optimized subset.
   * @param tasks The tasks to optimize.
   */
  SimpleOptimizationRule() : OptimizationRule(R::ids()) {}
  ~SimpleOptimizationRule() override = default;

  std::string getName() const override {
    return tasks::ID<typename R::Executor>::NAME;
  }

 protected:
  TaskList getReplacement(const TaskListRange& range) const override {
    auto ptr = R::initReplacement(range);
    TaskList list;
    list.emplace_back(std::move(ptr));
    return list;
  };
};

namespace simpleoptimizer {
struct FileMapperOptimization {
  using Executor = tasks::load::LSFileMapper;
  using Task = tasks::load::LSFileMapperTask;
  static std::vector<std::string> ids() {
    return {tasks::ID<tasks::load::LSFileOpener>::NAME,
            tasks::ID<tasks::load::LineSeparatedStreamReader>::NAME};
  }
  static std::unique_ptr<Task> initReplacement(
      const OptimizationRule::TaskListRange& range) {
    return std::make_unique<Task>();
  }
};
}  // namespace simpleoptimizer

using FileMapOptimization = SimpleOptimizationRule<simpleoptimizer::FileMapperOptimization>;

namespace chooseasoptimizer {
struct ChooseAsOptimizer {
  using Executor = tasks::optimization::ChooseAsPipeline;
  using Task = tasks::optimization::ChooseAsPipelineTask;
  static std::vector<std::string> ids() {
    return {tasks::ID<tasks::choose::ChoosePipeline>::NAME,
            tasks::ID<tasks::as::AsPipeline>::NAME};
  }
  static std::unique_ptr<Task> initReplacement(
      const OptimizationRule::TaskListRange& range) {
    auto const* choose =
        dynamic_cast<tasks::choose::ChoosePipelineTask*>(range.first->get());
    DCHECK(choose != nullptr) << "Got " << range.first->get()->getName()
                              << " expected ChoosePipeline";
    const auto& [query, executors] = choose->getData();
    return std::make_unique<Task>(executors, query);
  }
};
}  // namespace chooseasoptimizer

using ChooseAsOptimization =
    SimpleOptimizationRule<chooseasoptimizer::ChooseAsOptimizer>;

namespace chooseasaggoptimizer {
struct ChooseAsAggOptimizer {
  using Executor = tasks::optimization::ChooseAsAggPipeline;
  using Task = tasks::optimization::ChooseAsAggPipelineTask;
  static std::vector<std::string> ids() {
    return {tasks::ID<tasks::choose::ChoosePipeline>::NAME,
            tasks::ID<tasks::as::AsPipeline>::NAME,
            tasks::ID<tasks::agg::AggPipeline>::NAME};
  }
  static std::unique_ptr<Task> initReplacement(
      const OptimizationRule::TaskListRange& range) {
    auto const* choose =
        dynamic_cast<tasks::choose::ChoosePipelineTask*>(range.first->get());
    DCHECK(choose != nullptr) << "Got " << range.first->get()->getName()
                              << " expected ChoosePipeline";
    const auto& [query, executors] = choose->getData();
    return std::make_unique<Task>(executors, query);
  }
};
}  // namespace chooseasaggoptimizer

using ChooseAsAggOptimization =
    SimpleOptimizationRule<chooseasaggoptimizer::ChooseAsAggOptimizer>;

namespace chooseaggoptimizer {
struct ChooseAggOptimizer {
  using Executor = tasks::optimization::ChooseAggPipeline;
  using Task = tasks::optimization::ChooseAggPipelineTask;
  static std::vector<std::string> ids() {
    return {tasks::ID<tasks::choose::ChoosePipeline>::NAME,
            tasks::ID<tasks::agg::FilterAggPipeline>::NAME};
  }
  static std::unique_ptr<Task> initReplacement(
      const OptimizationRule::TaskListRange& range) {
    auto const* choose =
        dynamic_cast<tasks::choose::ChoosePipelineTask*>(range.first->get());
    DCHECK(choose != nullptr) << "Got " << range.first->get()->getName()
                              << " expected ChoosePipeline";
    const auto& [query, executors] = choose->getData();
    return std::make_unique<Task>(executors, query);
  }
};
}  // namespace chooseaggoptimizer

using ChooseAggOptimization =
    SimpleOptimizationRule<chooseaggoptimizer::ChooseAggOptimizer>;

}  // namespace joda::queryexecution::pipeline::optimization

#endif  // JODA_PIPELINE_SIMPLE_OPTIMIZATION_RULE_H