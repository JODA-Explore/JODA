#include <joda/join/relation/ContainerJoinExecutor.h>
#include <thread>

namespace joda::join {

  void ContainerJoinExecutor::setInnerFinished() {
    innerFinished.store(true);
  }

  void ContainerJoinExecutor::addInner(const std::shared_ptr<JSONContainer>& cont) {
    innerContainers.push_back(cont);
    handleInnerContainer(cont);
  }

  void ContainerJoinExecutor::handleInnerContainer(const std::shared_ptr<JSONContainer>& cont) {
  }

  std::shared_ptr<JSONContainer> ContainerJoinExecutor::joinContainer(
    const std::shared_ptr<JSONContainer>& cont) const {
      // Wait for inner to finish
      while(!innerFinished.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
      return handleOuterContainer(cont);
    }

} // namespace joda::join