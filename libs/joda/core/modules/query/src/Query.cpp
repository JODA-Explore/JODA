//
// Created by Nico Schäfer on 4/24/17.
//

#include "../include/joda/query/Query.h"

#include "../predicate/include/joda/query/predicate/CopyPredicateVisitor.h"
#include "../predicate/include/joda/query/predicate/ValToPredicate.h"
#include "../predicate/include/joda/query/predicate/ToStringVisitor.h"

bool joda::query::Query::check(const RapidJsonDocument &json) const {

  return pred->check(json);
}

void joda::query::Query::setPredicate(std::unique_ptr<Predicate> &&predicate) {
  pred = std::move(predicate);
}

std::unique_ptr<joda::query::Predicate> joda::query::Query::getPredicate() const {
  CopyPredicateVisitor copy;
  pred->accept(copy);
  return copy.getPredicate();
}

void joda::query::Query::addAggregator(std::unique_ptr<IAggregator> &&agg) {
  aggregators.push_back(std::move(agg));
}

bool joda::query::Query::hasAggregators() {
  return !aggregators.empty();
}

joda::query::Query::Query() {
  setPredicate(std::make_unique<ValToPredicate>(true));
}

const std::vector<std::unique_ptr<joda::query::IAggregator>> &joda::query::Query::getAggregators() const {
  return aggregators;
}

void joda::query::Query::addProjection(std::unique_ptr<IProjector> &&expr) {
  projectors.push_back(std::move(expr));
}

const std::vector<std::unique_ptr<joda::query::IProjector>> &joda::query::Query::getProjectors() const {
  return projectors;
}

const std::string &joda::query::Query::getLoad() const {
  return load;
}

void joda::query::Query::setLoad(const std::string &load) {
  joda::query::Query::load = load;
}

const std::string &joda::query::Query::getDelete() const {
  return del;
}

void joda::query::Query::setDelete(const std::string &del) {
  Query::del = del;
}

bool joda::query::Query::isDefault() const {
  //No special query happens
  bool val;
  return chooseIsConst(val) && val && aggregators.empty() && projectors.empty() && setProjectors.empty()
      && exportDestination == nullptr;
}

const std::vector<std::unique_ptr<joda::query::ISetProjector>> &joda::query::Query::getSetProjectors() const {
  return setProjectors;
}

void joda::query::Query::addProjection(std::unique_ptr<ISetProjector> &&expr) {
  setProjectors.push_back(std::move(expr));
}

bool joda::query::Query::chooseIsConst(bool &val) const {
  auto *predVal = dynamic_cast<ValToPredicate *> (pred.get());
  if (predVal != nullptr) {
    auto pvTrue = predVal->isConstTrue();
    auto pvFalse = predVal->isConstFalse();
    val = pvTrue;
    return pvTrue || pvFalse;
  }
  return false;
}

std::string joda::query::Query::toString() const {
  //LOadVar
  std::string ret = "LOAD " + getLoad();

  //LoadFIle
  for (const auto &importSource : importSources) {
    ret += " " + importSource->toQueryString();
  }
  if (loadJoinManager != nullptr) {
    ret += " FROM GROUPED " + loadJoinManager->getName();
  }


  //CHOOSE
  bool choose;
  if (!(chooseIsConst(choose) && choose)) {
    ToStringVisitor chooseString;
    pred->accept(chooseString);
    ret += " CHOOSE " + chooseString.popString();
  }

  //AS (Flat)
  if (projectors.size() + setProjectors.size() != 0) {
    ret += " AS ";
    for (auto &&item : projectors) {
      ret += "(" + item->toString() + "),";
    }
    for (auto &&item : setProjectors) {
      ret += "(" + item->toString() + "),";
    }
    ret = ret.substr(0, ret.size() - 1);
  }

  //AGG
  if (!aggregators.empty()) {
    ret += " AGG ";
    for (auto &&item : aggregators) {
      ret += "(" + item->toString() + "),";
    }
    ret = ret.substr(0, ret.size() - 1);
  }

  //STORE VAR
  if (exportDestination != nullptr) {
    ret += exportDestination->toQueryString();
  }

  //Delete
  if (!del.empty()) ret += " DELETE " + del;
  return ret;
}

const std::shared_ptr<JoinManager> &joda::query::Query::getLoadJoinManager() const {
  return loadJoinManager;
}

void joda::query::Query::setLoadJoinManager(const std::shared_ptr<JoinManager> &loadJoinManager) {
  joda::query::Query::loadJoinManager = loadJoinManager;
}

const std::shared_ptr<JoinManager> &joda::query::Query::getStoreJoinManager() const {
  return storeJoinManager;
}

void joda::query::Query::setStoreJoinManager(const std::shared_ptr<JoinManager> &storeJoinManager) {
  Query::storeJoinManager = storeJoinManager;
}

const std::vector<std::unique_ptr<joda::docparsing::IImportSource>> &joda::query::Query::getImportSources() const {
  return importSources;
}

void joda::query::Query::addImportSource(std::unique_ptr<joda::docparsing::IImportSource> &&source) {
  importSources.push_back(std::move(source));
}

std::unique_ptr<IExportDestination> &joda::query::Query::getExportDestination() {
  return exportDestination;
}

void joda::query::Query::setExportDestination(std::unique_ptr<IExportDestination> &&exportDestination) {
  joda::query::Query::exportDestination = std::move(exportDestination);
}
