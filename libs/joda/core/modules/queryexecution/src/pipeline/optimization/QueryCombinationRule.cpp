#include "QueryCombinationRule.h"
namespace joda::queryexecution::pipeline::optimization {
bool QueryCombinationRule::validateOptimizationTarget(
    const TaskListRange& range) {
  auto* receiver =
      dynamic_cast<tasks::storage::StorageReceiverTask*>(range.first->get());
  auto* sender =
      dynamic_cast<tasks::storage::StorageSenderTask*>(range.second->get());

  if (sender == nullptr || receiver == nullptr) {
    // Wrong types
    return false;
  }

  // Get data from senders
  auto senderData = sender->getData();
  auto receiverData = receiver->getData();
  auto senderName = std::get<0>(senderData);
  auto receiverName = std::get<0>(receiverData);

  if (!senderName.empty() && !receiverName.empty() &&
      senderName == receiverName) {
    // Same name
    name = senderName;
    return true;
  }
  return false;
}

OptimizationRule::TaskList QueryCombinationRule::getReplacement(
    const TaskListRange& range) const {
  DCHECK(!name.empty());
  auto ptr = std::make_unique<tasks::storage::StorageBufferTask>(name);
  TaskList list;
  list.emplace_back(std::move(ptr));
  return list;
}

void QueryCombinationRule::reset() {
  name.clear();
}

}  // namespace joda::queryexecution::pipeline::optimization