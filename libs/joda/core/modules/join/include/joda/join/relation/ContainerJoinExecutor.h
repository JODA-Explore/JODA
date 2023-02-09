#ifndef JODA_CONTAINER_JOIN_EXECUTOR_H
#define JODA_CONTAINER_JOIN_EXECUTOR_H

#include <joda/container/JSONContainer.h>

namespace joda::join {
class ContainerJoinExecutor {
 public:
  virtual ~ContainerJoinExecutor() = default;

  /**
   * Adds a container of the inner relation
   */
  void addInner(const std::shared_ptr<JSONContainer>& cont);

  /**
   * Inner relation is finished.
   * Enables joins of outer
   */
  void setInnerFinished();

  /**
   * Joins one container to all the inner containers
   * @param cont The container to join
   * @return The joined container
   */
  std::shared_ptr<JSONContainer> joinContainer(
      const std::shared_ptr<JSONContainer>& cont) const;

  virtual std::string toString() const = 0;

 protected:
  // The list of containers of the inner relation
  std::vector<std::shared_ptr<JSONContainer>> innerContainers{};
  // Wether or not the inner relation is finished with indexing
  std::atomic_bool innerFinished{};

  /**
   * Handles / indexes / ... the the containers of the inner join relation
   */
  virtual void handleInnerContainer(const std::shared_ptr<JSONContainer>& cont);

  virtual std::shared_ptr<JSONContainer> handleOuterContainer(
      const std::shared_ptr<JSONContainer>& cont) const = 0;
};

}  // namespace joda::join

#endif  // JODA_CONTAINER_JOIN_EXECUTOR_H