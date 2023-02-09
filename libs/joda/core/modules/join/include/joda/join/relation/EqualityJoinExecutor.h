#ifndef JODA_CONTAINER_EQUALITY_JOIN_EXECUTOR_H
#define JODA_CONTAINER_EQUALITY_JOIN_EXECUTOR_H

#include <joda/join/relation/ContainerJoinExecutor.h>
#include <joda/query/values/EqualityProvider.h>

namespace joda::join {
class EqualityJoinExecutor : public ContainerJoinExecutor {
 public:
  EqualityJoinExecutor(std::unique_ptr<query::IValueProvider>&& inner,
                       std::unique_ptr<query::IValueProvider>&& outer);

  ~EqualityJoinExecutor() override = default;

  virtual std::string toString() const override;

 protected:
  std::unique_ptr<query::EqualProvider> pred;
  std::vector<std::string> innerAtts;
  std::vector<std::string> outerAtts;

  virtual std::shared_ptr<JSONContainer> handleOuterContainer(
      const std::shared_ptr<JSONContainer>& cont) const override;
};

}  // namespace joda::join

#endif  // JODA_CONTAINER_EQUALITY_JOIN_EXECUTOR_H