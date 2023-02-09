//
// Created by Nico Schäfer on 4/24/17.
//

#include "../include/joda/query/Query.h"

#include <joda/query/project/PointerCopyProject.h>
#include <joda/query/values/AtomProvider.h>

bool joda::query::Query::check(const RapidJsonDocument& json) const {
  RJMemoryPoolAlloc alloc;
  if (!pred->isAtom()) {
    auto ret = pred->getValue(json, alloc);
    if (ret == nullptr) {
      return false;
    }
    if (!ret->IsBool()) {
      return false;
    }
    return ret->GetBool();
  } else {
    auto ret = pred->getAtomValue(json, alloc);
    if (!ret.IsBool()) {
      return false;
    }
    return ret.GetBool();
  }
  return false;
}

void joda::query::Query::setChoose(
    std::unique_ptr<IValueProvider>&& predicate) {
  DCHECK(predicate->isBool() || predicate->isAny()) << "Choose predicate must be a boolean";
  pred = std::move(predicate);
}

std::unique_ptr<joda::query::IValueProvider> joda::query::Query::getChoose()
    const {
  return pred->duplicate();
}

void joda::query::Query::addAggregator(std::unique_ptr<IAggregator>&& agg) {
  aggregators.push_back(std::move(agg));
}

bool joda::query::Query::hasAggregators() const { return !aggregators.empty(); }

void joda::query::Query::setAggWindowSize(uint64_t size) {
  aggWindowSize = size;
}

uint64_t joda::query::Query::getAggWindowSize() const { return aggWindowSize; }

bool joda::query::Query::hasAggWindow() const { return aggWindowSize != 0; }

joda::query::Query::Query() { setChoose(std::make_unique<BoolProvider>(true)); }

const std::vector<std::unique_ptr<joda::query::IAggregator>>&
joda::query::Query::getAggregators() const {
  return aggregators;
}

void joda::query::Query::addProjection(std::unique_ptr<IProjector>&& expr) {
  projectors.push_back(std::move(expr));
}

const std::vector<std::unique_ptr<joda::query::IProjector>>&
joda::query::Query::getProjectors() const {
  return projectors;
}

const std::string& joda::query::Query::getLoad() const { return load; }

void joda::query::Query::setLoad(const std::string& load) {
  joda::query::Query::load = load;
}

bool joda::query::Query::isDefault() const {
  // No special query happens
  return !hasChoose() && !hasAggregators() && !hasAS() && !hasJOIN() &&
         exportDestination == nullptr;
}

bool joda::query::Query::hasChoose() const {
  bool val;
  return !(chooseIsConst(val) && val);
}

bool joda::query::Query::hasAS() const {
  return !projectors.empty() || !setProjectors.empty();
}

const std::vector<std::unique_ptr<joda::query::ISetProjector>>&
joda::query::Query::getSetProjectors() const {
  return setProjectors;
}

void joda::query::Query::addProjection(std::unique_ptr<ISetProjector>&& expr) {
  setProjectors.push_back(std::move(expr));
}

bool joda::query::Query::chooseIsConst(bool& val) const {
  if(!pred->isConst()) {
    return false;
  }
  RapidJsonDocument doc;
  val = check(doc);
  return true;
}

std::string joda::query::Query::toString() const {
  // LOadVar
  std::string ret = "LOAD " + getLoad();

  // LoadFIle
  for (const auto& importSource : importSources) {
    ret += " " + importSource->toQueryString();
  }
  if (loadJoinManager != nullptr) {
    ret += " FROM GROUPED " + loadJoinManager->getName();
  }

  // CHOOSE
  bool choose;
  if (!(chooseIsConst(choose) && choose)) {
    ret += " CHOOSE " + pred->toString();
  }

  if (hasJOIN()) {
    ret += " JOIN ";
    if (subQuery != nullptr) {
      ret += "(";
      ret += subQuery->toString();
      ret += ")";
    } else {
      ret += joinPartner;
    }
    ret += " " + joinExecutor->toString();
  }

  // AS (Flat)
  if (projectors.size() + setProjectors.size() != 0) {
    ret += " AS ";
    for (auto&& item : projectors) {
      ret += "(" + item->toString() + "),";
    }
    for (auto&& item : setProjectors) {
      ret += "(" + item->toString() + "),";
    }
    ret = ret.substr(0, ret.size() - 1);
  }

  // AGG
  if (!aggregators.empty()) {
    ret += " AGG ";
    for (auto&& item : aggregators) {
      ret += "(" + item->toString() + "),";
    }
    ret = ret.substr(0, ret.size() - 1);
  }

  // STORE VAR
  if (exportDestination != nullptr) {
    ret += " " + exportDestination->toQueryString();
  }

  return ret;
}

const std::shared_ptr<JoinManager>& joda::query::Query::getLoadJoinManager()
    const {
  return loadJoinManager;
}

void joda::query::Query::setLoadJoinManager(
    const std::shared_ptr<JoinManager>& loadJoinManager) {
  joda::query::Query::loadJoinManager = loadJoinManager;
}

const std::shared_ptr<JoinManager>& joda::query::Query::getStoreJoinManager()
    const {
  return storeJoinManager;
}

void joda::query::Query::setStoreJoinManager(
    const std::shared_ptr<JoinManager>& storeJoinManager) {
  joda::query::Query::storeJoinManager = storeJoinManager;
}

const std::vector<std::unique_ptr<joda::docparsing::IImportSource>>&
joda::query::Query::getImportSources() const {
  return importSources;
}

void joda::query::Query::addImportSource(
    std::unique_ptr<joda::docparsing::IImportSource>&& source) {
  importSources.push_back(std::move(source));
}

const std::unique_ptr<IExportDestination>&
joda::query::Query::getExportDestination() const {
  return exportDestination;
}

void joda::query::Query::setExportDestination(
    std::unique_ptr<IExportDestination>&& exportDestination) {
  joda::query::Query::exportDestination = std::move(exportDestination);
}

std::vector<std::string> joda::query::Query::getAllUsedAttributes() const {
  // Get Choose attributes
  std::vector<std::string> ret = pred->getAttributes();
  // Get AS Attributes
  for (const auto& item : projectors) {
    auto tmp = item->getMaterializeAttributes();
    std::move(tmp.begin(), tmp.end(), std::back_inserter(ret));
  }
  for (const auto& item : setProjectors) {
    auto tmp = item->getAttributes();
    std::move(tmp.begin(), tmp.end(), std::back_inserter(ret));
  }
  // Get AGG Attributes
  for (const auto& item : aggregators) {
    auto tmp = item->getAttributes();
    std::move(tmp.begin(), tmp.end(), std::back_inserter(ret));
  }
  std::sort(ret.begin(), ret.end());
  ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
  return ret;
}

std::vector<std::string> joda::query::Query::getChooseAttributes() const {
  std::vector<std::string> ret = pred->getAttributes();
  std::sort(ret.begin(), ret.end());
  ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
  return ret;
}

std::vector<std::string> joda::query::Query::getASAttributes() const {
  std::vector<std::string> ret;
  for (const auto& item : projectors) {
    auto tmp = item->getMaterializeAttributes();
    std::move(tmp.begin(), tmp.end(), std::back_inserter(ret));
  }
  for (const auto& item : setProjectors) {
    auto tmp = item->getAttributes();
    std::move(tmp.begin(), tmp.end(), std::back_inserter(ret));
  }
  std::sort(ret.begin(), ret.end());
  ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
  return ret;
}

std::vector<std::string> joda::query::Query::getAGGAttributes() const {
  std::vector<std::string> ret;
  for (const auto& item : aggregators) {
    auto tmp = item->getAttributes();
    std::move(tmp.begin(), tmp.end(), std::back_inserter(ret));
  }
  std::sort(ret.begin(), ret.end());
  ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
  return ret;
}

bool joda::query::Query::canCreateView() const {
  return config::enable_views &&
         ((!projectors.empty() &&
           projectors.front()->getType() ==
               PointerCopyProject::allCopy)                 // Delta Tree
          || (projectors.empty() && setProjectors.empty())  // Star expression
         );
}

bool joda::query::Query::hasJOIN() const { return joinExecutor != nullptr; }

std::shared_ptr<joda::join::ContainerJoinExecutor>
joda::query::Query::getJoinExecutor() const {
  return joinExecutor;
}

void joda::query::Query::setJoinExecutor(
    std::shared_ptr<joda::join::ContainerJoinExecutor> joinExecutor) {
  joda::query::Query::joinExecutor = std::move(joinExecutor);
}

std::string joda::query::Query::getJoinPartner() const { return joinPartner; }

void joda::query::Query::setJoinPartner(const std::string& joinPartner) {
  joda::query::Query::joinPartner = joinPartner;
}

void joda::query::Query::setSubQuery(
    std::shared_ptr<joda::query::Query> subquery) {
  joda::query::Query::subQuery = std::move(subquery);
}

std::shared_ptr<joda::query::Query> joda::query::Query::getSubQuery() const {
  return subQuery;
}