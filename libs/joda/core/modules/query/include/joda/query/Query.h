//
// Created by Nico Schäfer on 4/24/17.
//

#ifndef JODA_QUERY_H
#define JODA_QUERY_H

#include <joda/document/RapidJsonDocument.h>
#include <joda/export/IExportDestination.h>
#include <joda/join/JoinManager.h>
#include <joda/join/relation/ContainerJoinExecutor.h>
#include <joda/parser/IImportSource.h>
#include <joda/query/aggregation/IAggregator.h>
#include <joda/query/project/IProjector.h>
#include <joda/query/project/ISetProjector.h>

#include <memory>

namespace joda::query {
/**
 * This class represents the parsed representation of a JODA Query.
 *
 * It contains all information required to execute a user query.
 */
class Query {
 public:
  /**
   * The default constructor constructs an empty default query.
   */
  Query();

  /**
   * This function checks if the given document fulfils the CHOOSE predicate of
   * the query.
   * @param doc The document to check
   * @return True if the document fulfils the predicate, else False
   */
  bool check(const RapidJsonDocument &json) const;

  /**
   * @return the name of the collection on which this query is based.
   */
  const std::string &getLoad() const;

  /**
   * @param load  the name of the collection on which this query is based.
   */
  void setLoad(const std::string &load);

  /**
   * Sets the join executor of the JOIN command.
   */
  void setJoinExecutor(std::shared_ptr<join::ContainerJoinExecutor> executor);

  /**
   * Returns the join executor of the JOIN command, if any.
   */
  std::shared_ptr<join::ContainerJoinExecutor> getJoinExecutor() const;

  /*
   * Sets the join partner of the JOIN command.
   */
  void setJoinPartner(const std::string &partner);

  /**
   * Returns the join partner of the JOIN command, if any.
   */
  std::string getJoinPartner() const;

  /**
   * Sets the subquery of the JOIN command.
   */
  void setSubQuery(std::shared_ptr<Query> subquery);

  /**
   * Returns the subquery of the JOIN command, if any.
   */
  std::shared_ptr<Query> getSubQuery() const;

  /**
   *
   * @param predicate the predicate of the CHOOSE expression
   */
  void setChoose(std::unique_ptr<IValueProvider> &&predicate);

  /**
   *
   * @return a copy of the CHOOSE expression predicate, default:
   * TrueValue
   */
  std::unique_ptr<IValueProvider> getChoose() const;

  /**
   * Adds a Projection expression to the query
   * @param expr The projection to add
   */
  void addProjection(std::unique_ptr<IProjector> &&expr);
  /**
   *
   * @return the list of projections
   */
  const std::vector<std::unique_ptr<IProjector>> &getProjectors() const;

  /**
   * Adds a SetProjection expression to the query
   * @param expr The setprojection to add
   */
  void addProjection(std::unique_ptr<ISetProjector> &&expr);

  /**
   *
   * @return the list of setProjections
   */
  const std::vector<std::unique_ptr<ISetProjector>> &getSetProjectors() const;

  /**
   * Adds a Aggregator to the query
   * @param agg The aggregator to add
   */
  void addAggregator(std::unique_ptr<IAggregator> &&agg);

  /**
   * @return True if the query contains an aggregation, else False
   */
  bool hasAggregators() const;

  /**
   * Sets the aggregation window size
   * @param size The size of the aggregation window, 0 means no window
   */
  void setAggWindowSize(uint64_t size);

  /**
   * Gets the aggregation window size
   * @return The size of the aggregation window, 0 means no window
   * @see setAggWindowSize
   */
  uint64_t getAggWindowSize() const;

  /**
   * Returns wether or not the query contains an aggregation window
   * @return True if the query contains an aggregation window, else False
   * @see getAggWindowSize
   */
  bool hasAggWindow() const;

  /**
   *
   * @return The list of aggregators, default: []
   */
  const std::vector<std::unique_ptr<IAggregator>> &getAggregators() const;
  const std::vector<std::unique_ptr<docparsing::IImportSource>>
      &getImportSources() const;
  void addImportSource(std::unique_ptr<docparsing::IImportSource> &&source);
  const std::shared_ptr<JoinManager> &getLoadJoinManager() const;
  void setLoadJoinManager(const std::shared_ptr<JoinManager> &loadJoinManager);
  const std::shared_ptr<JoinManager> &getStoreJoinManager() const;
  void setStoreJoinManager(
      const std::shared_ptr<JoinManager> &storeJoinManager);
  const std::unique_ptr<IExportDestination> &getExportDestination() const;
  void setExportDestination(
      std::unique_ptr<IExportDestination> &&exportDestination);

  /**
   *
   * @return True if this query is a NOOP query (does nothing except for
   * displaying a collection), else False
   */
  bool isDefault() const;

  /**
   * Checks whether the CHOOSE predicate can be evaluated without documents.
   * If yes, the result of the predicate is written to val.
   * @param val A variable which will contain the result of the const predicate.
   * @return True if the predicate is const, else False
   */
  bool chooseIsConst(bool &val) const;

  /**
   * Checks whether the CHOOSE predicate has to be evaluated
   * @return true if the predicate has to be evaluated, else false
   */
  bool hasChoose() const;

  /**
   * Checks whether the AS statement has to be evaluated
   * @return true if the statement has to be evaluated, else false
   */
  bool hasAS() const;

  /**
   * Checks whether the JOIN statement has to be evaluated
   * @return true if the statement has to be evaluated, else false
   */
  bool hasJOIN() const;

  /**
   * Checks whether the AS transformations can be evaluated with views.
   * @return
   */
  bool canCreateView() const;

  /**
   * @return the string representation of the query.
   */
  std::string toString() const;

  std::vector<std::string> getAllUsedAttributes() const;
  std::vector<std::string> getChooseAttributes() const;
  std::vector<std::string> getASAttributes() const;
  std::vector<std::string> getAGGAttributes() const;

 protected:
  /*
   * Load command
   */
  std::vector<std::unique_ptr<docparsing::IImportSource>> importSources{};
  std::string load{};

  std::shared_ptr<JoinManager> loadJoinManager = nullptr;

  /*
   * Join command
   */
  std::shared_ptr<join::ContainerJoinExecutor> joinExecutor;
  std::string joinPartner;
  std::shared_ptr<Query> subQuery;

  /*
   * Choose command
   */
  std::unique_ptr<IValueProvider> pred = nullptr;

  /*
   * AS command
   */
  std::vector<std::unique_ptr<IProjector>> projectors{};
  std::vector<std::unique_ptr<ISetProjector>> setProjectors{};

  /*
   * AGG command
   */

  std::vector<std::unique_ptr<IAggregator>> aggregators{};
  uint64_t aggWindowSize = 0;

  /*
   * Store command
   */
  std::unique_ptr<IExportDestination> exportDestination = nullptr;
  std::shared_ptr<JoinManager> storeJoinManager = nullptr;
  std::string store;
};
}  // namespace joda::query

#endif  // JODA_QUERY_H
