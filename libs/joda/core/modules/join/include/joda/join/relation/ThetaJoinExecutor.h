#ifndef JODA_CONTAINER_THETA_JOIN_EXECUTOR_H
#define JODA_CONTAINER_THETA_JOIN_EXECUTOR_H

#include <joda/join/relation/ContainerJoinExecutor.h>

namespace joda::join {

class WherePredicateNotBoolException : public std::exception {
 public:
  WherePredicateNotBoolException() = default;
  const char* what() const throw() {
    return "The WHERE predicate is not of type Boolean";
  }
};

class ThetaJoinExecutor : public ContainerJoinExecutor {
 public:
  ThetaJoinExecutor(std::unique_ptr<query::IValueProvider>&& predicate);

  ~ThetaJoinExecutor() override = default;

  virtual std::string toString() const override;

 protected:
  std::unique_ptr<query::IValueProvider> predicate;

  virtual std::shared_ptr<JSONContainer> handleOuterContainer(
      const std::shared_ptr<JSONContainer>& cont) const override;
};

}  // namespace joda::join

#endif  // JODA_CONTAINER_THETA_JOIN_EXECUTOR_H