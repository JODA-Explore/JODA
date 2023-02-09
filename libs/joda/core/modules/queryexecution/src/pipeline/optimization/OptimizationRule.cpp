
#include "OptimizationRule.h"

#include <unordered_set>
#include <algorithm>

namespace joda::queryexecution::pipeline::optimization {

OptimizationRule::TaskListRange OptimizationRule::findOptimizationTarget(
    TaskList& tasks,
    const std::vector<std::pair<tasks::TaskNum, tasks::TaskNum>>& connections)
    {
  auto found = tasks.begin();
  do {
    // Search for potential subset
    found = std::search(
        found, tasks.end(), toOptimize.begin(), toOptimize.end(),
        [](const tasks::PipelineTaskPtr& a, const tasks::TaskID& b) {
          return a->getName() == b;
        });

    if (found == tasks.end()) {
      // No subset found
      break;
    }

    // Potential task set found, check that there are no outside connections of
    // inner tasks
    std::unordered_set<tasks::TaskNum> task_ids;
    auto task_it = found;
    auto outside_connections = false;
    for (auto it = toOptimize.begin(); it != toOptimize.end(); ++it) {
      auto task_num = (*task_it)->getNum();
      // Add task to set of tasks to optimize
      task_ids.insert(task_num);

      // Do not check outside connections of first task, as it has expected
      // outside connections
      if (task_ids.size() > 1) {

        // Find if there is a connection from an outside task (not in the
        // task_id set) to the current task
        auto outside = std::ranges::find_if(connections,
                         [&task_ids, task_num](
                             const std::pair<tasks::TaskNum, tasks::TaskNum>&
                                 connection) {
                           return connection.second == task_num &&
                                  !task_ids.contains(connection.first);
                         });
        if (outside == connections.end()) {
          // No outside connection found => continue checking
          continue;
        }else{
          DLOG(INFO) << toString() << " rejected, because of connection from outside task " << outside->first << " to " << outside->second;
          outside_connections = true;
          break;
        }
      }

      task_it++;  // Next task
    }
    if(!outside_connections){
      // No outside connections found => return result
      auto end = found;
      for (auto it = toOptimize.begin()+1; it != toOptimize.end(); ++it) {
        end++;
      }

      TaskListRange potentialRange = std::make_pair(found, end);
      // Check wether the potential range is valid, depending on the content
      if(validateOptimizationTarget(potentialRange)){
        return potentialRange;
      } 
    }

    // If not found, continue searching
    found++;
  } while (found != tasks.end());

  DLOG(INFO) << toString() << " rejected, because no optimization target was found";
  return  std::make_pair(tasks.end(), tasks.end());
}

std::string OptimizationRule::toString() const {
  std::stringstream ss;
  ss << "OptimizationRule " << getName() << ": [";
  for (auto it = toOptimize.begin(); it != toOptimize.end(); ++it) {
    if (it != toOptimize.begin()) {
      ss << ", ";
    }
    ss << *it;
  }
  ss << "]";
  return ss.str();
}

std::optional<OptimizationRule::Replacement> OptimizationRule::optimize(
      TaskList& tasks, const std::vector<std::pair<tasks::TaskNum,tasks::TaskNum>>& connections) {
  // Find optimization target
  auto range = findOptimizationTarget(tasks, connections);
  if (range.first == tasks.end()) { // No replacement found
    reset();
    return std::nullopt;
  }

  // Get replacements
  auto replacements = getReplacement(range);
  reset();
  if (replacements.empty()) {
    return std::nullopt;
  }

  // Build replacement object
  return std::make_pair(std::move(range), std::move(replacements));
}


}  // namespace joda::queryexecution::pipeline::optimization
