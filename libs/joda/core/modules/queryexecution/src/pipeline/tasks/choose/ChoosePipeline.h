#ifndef JODA_PIPELINE_CHOOSEPIPELINE_H
#define JODA_PIPELINE_CHOOSEPIPELINE_H

#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/storage/JSONStorage.h>

#include <joda/queryexecution/IQueryExecutor.h>

namespace joda::queryexecution::pipeline::tasks::choose {

class FilteredJSONContainer {
 public:
 FilteredJSONContainer() = default;
  FilteredJSONContainer(std::shared_ptr<JSONContainer> cont, bool constFilter)
      : container(cont), filter(constFilter) {
    DCHECK(filter.index() != std::variant_npos);
  };
  FilteredJSONContainer(std::shared_ptr<JSONContainer> cont,
                        std::shared_ptr<const DocIndex>&& index)
      : container(cont), filter(std::move(index)) {
    DCHECK(filter.index() != std::variant_npos);
  };

  const std::shared_ptr<JSONContainer>& getContainer() const {
    return container;
  };

  bool isConst() const { return std::holds_alternative<bool>(filter); }

  bool getConstFilter() const {
    DCHECK(isConst()) << "Should check beforehand if this is a const filter";
    return std::get<bool>(filter);
  }

  const std::shared_ptr<const DocIndex>& getIndex() const {
    DCHECK(!isConst()) << "Should check beforehand if this is a const filter";
    DCHECK(filter.index() != std::variant_npos);
    return std::get<std::shared_ptr<const DocIndex>>(filter);
  }

 private:
  std::shared_ptr<JSONContainer> container;
  // If bool => choose everything if true, else choose nothing
  // If docindex => mixture
  std::variant<bool, std::shared_ptr<const DocIndex>> filter;
};

/**
 * Performs the CHOOSE operation on one container
 */
class ChooseExec {
 public:
  typedef queue::InputQueueTrait<std::shared_ptr<JSONContainer>> I;
  typedef queue::OutputQueueTrait<FilteredJSONContainer> O;

  // Overwrite Input Bulk Size
  static constexpr size_t InputBulk = 1;

  ChooseExec(const std::vector<std::unique_ptr<IQueryExecutor>>& execs,
             const std::shared_ptr<const query::Query>& query);
  ChooseExec(const ChooseExec& other);
  ChooseExec(ChooseExec&& other) = delete;

  void fillBuffer(std::optional<I::Input>& ibuff, std::optional<O::Output>& obuff,
                  std::function<void(std::optional<O::Output>&)> sendPartial);

  void finalize(std::optional<O::Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial);

  /**
   * @brief Filters a single container
   *
   * @param input The input container
   * @return std::vector<O::Output> The filtered container
   */
  std::optional<O::Output> handleOne(I::Input& input);

  std::string toString() const;
  std::pair< std::shared_ptr<const query::Query>, std::vector<std::unique_ptr<IQueryExecutor>>> getData() const;

 private:
  std::vector<std::unique_ptr<IQueryExecutor>> executors;
  std::shared_ptr<const query::Query> query;
  std::vector<std::string> chooseAttributes;
};

typedef IOExecutor<ChooseExec, tasks::AsyncType::MultiThreaded> ChoosePipeline;

}  // namespace joda::queryexecution::pipeline::tasks::choose

JODA_REGISTER_PIPELINE_TASK_IN_NS(ChoosePipeline, "ChoosePipeline",
                                  joda::queryexecution::pipeline::tasks::choose)

#endif  // JODA_PIPELINE_CHOOSEPIPELINE_H