#ifndef JODA_PIPELINE_OPTIMIZATION_RULE_H
#define JODA_PIPELINE_OPTIMIZATION_RULE_H

#include <joda/pipelineatomics/PipelineTask.h>

#include <list>
#include <vector>
#include <optional>

namespace joda::queryexecution::pipeline::optimization {

/**
 * @brief The OptimizationRule class, which represents a set of tasks that may be replaced by different tasks to optimize the pipeline
 */
class OptimizationRule {
 public:
  typedef std::list<tasks::PipelineTaskPtr> TaskList;
  typedef typename TaskList::iterator TaskListIt;
  typedef std::pair<TaskListIt,TaskListIt> TaskListRange;
  typedef std::pair<TaskListRange,TaskList> Replacement;

  /**
   * Creates a new Optimization rule which replace the given set of tasks with a
   * optimized subset.
   * @param tasks The tasks to optimize.
   */
  explicit OptimizationRule(std::vector<tasks::TaskID>&& ids) : toOptimize(std::move(ids)) {
    DCHECK(!toOptimize.empty()) << "Optimization rule can't be empty";
  }
   explicit OptimizationRule(const std::vector<tasks::TaskID>& ids) : toOptimize(ids) {
    DCHECK(!toOptimize.empty()) << "Optimization rule can't be empty";
  }
  virtual ~OptimizationRule() = default;

  /**
   * @brief Optimizes the pipeline by replacing the given set of tasks with a optimized subset.
   * @param tasks The pipeline to optimize.
   * @return Replacement The replacement
   */
  virtual std::optional<Replacement> optimize(
      TaskList& tasks, const std::vector<std::pair<tasks::TaskNum,tasks::TaskNum>>& connections) ;
  

  /**
   * @brief Returns the name of the optimization rule
   * @return std::string The name of the optimization rule
   */
  virtual std::string getName() const = 0;

  /**
   * @brief Returns the description of the optimization rule
   * @return std::string The description of the optimization rule
   */
  virtual std::string toString() const;



 protected:
  // The tasks that are to be optimized
  std::vector<tasks::TaskID> toOptimize;

    /**
   * @brief Resets the rule to be reused
   *  Will happen after every optimization run
   */
  virtual void reset() {
    // Nothing to reset
  };

  /**
   * @brief Finds the [start,end] range of the tasks to be replaced
   */
  TaskListRange findOptimizationTarget(TaskList& tasks, const std::vector<std::pair<tasks::TaskNum,tasks::TaskNum>>& connections);

  /**
   * @brief Checks wether the optimizationtarget is valid.
   * This function can be used to check wether the optimization target can actually be used depending on the content of the tasks
   */
  virtual bool validateOptimizationTarget(const TaskListRange& range) {return true;}

  /**
   * @brief Returns the tasks which will replace the input tasks
   */
  virtual TaskList getReplacement(const TaskListRange& range) const = 0;
};

}  // namespace joda::queryexecution::pipeline::optimization

#endif  // JODA_PIPELINE_OPTIMIZATION_RULE_H