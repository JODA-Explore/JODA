#include "ChoosePipeline.h"

#include <joda/query/Query.h>

namespace joda::queryexecution::pipeline::tasks::choose {

void ChooseExec::fillBuffer(std::optional<I::Input>& ibuff, std::optional<O::Output>& obuff,
                  std::function<void(std::optional<O::Output>&)> sendPartial) {
  if(ibuff.has_value()) {
    auto cont = std::move(ibuff.value());
    ibuff.reset();
    if (cont == nullptr) {
      return;
    }

    auto o = handleOne(cont); //Filter documents
    if(o.has_value()){
      obuff = std::move(*o);
    }

    sendPartial(obuff);
  }
}

std::optional<ChooseExec::O::Output> ChooseExec::handleOne(I::Input& input) {
  auto contLock = input->useContInScope(false);

  // Materialize Delta Tree if required
  if (input->isView()) {
    input->materializeAttributesIfRequired(chooseAttributes);
  }

  // Get Executor
  IQueryExecutor* exec = nullptr;
  unsigned long estSize = NOT_APPLICABLE;

  for (auto&& executor : executors) {
    CHECK(executor != nullptr);
    auto s = executor->estimatedWork(*query, *input);
    if (s < estSize) {
      estSize = s;
      exec = executor.get();
    }
  }
  DCHECK(exec != nullptr);
  DLOG(INFO) << "Chose " << exec->getName() << " in "
             << std::this_thread::get_id();

  // Filter
  auto selectResult = exec->execute(*query, *input);

  for (auto&& executor : executors) {
    executor->alwaysAfterSelect(*query, selectResult, *input);
  }

  // Create Filtered Cont
  if (selectResult == nullptr || selectResult->empty()) {
    return {};
  } else {
    size_t selCount =
        selectResult->count();
    if (selCount == selectResult->size()) {
      return {O::Output(std::move(input), true)};
    } else if (selCount == 0) {
      return {};
    } else {
      return {O::Output(std::move(input), std::move(selectResult))};
    }
  }
}

void ChooseExec::finalize(std::optional<O::Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial) {}

ChooseExec::ChooseExec(
    const std::vector<std::unique_ptr<IQueryExecutor>>& execs,
    const std::shared_ptr<const query::Query>& query)
    : executors(),
      query(query),
      chooseAttributes(query->getChooseAttributes()) {
  for (const auto& exec : execs) {
    executors.emplace_back(exec->duplicate());
  }
}

ChooseExec::ChooseExec(const ChooseExec& other)
    : ChooseExec(other.executors, other.query) {}

std::string ChooseExec::toString() const {
  return query->getChoose()->toString();
}

std::pair< std::shared_ptr<const query::Query>, std::vector<std::unique_ptr<IQueryExecutor>>>  ChooseExec::getData() const {
  std::vector<std::unique_ptr<IQueryExecutor>> copy;
  for (const auto& exec : executors) {
    copy.emplace_back(exec->duplicate());
  }
  return std::make_pair(query, std::move(copy));
}

}  // namespace joda::queryexecution::pipeline::tasks::choose