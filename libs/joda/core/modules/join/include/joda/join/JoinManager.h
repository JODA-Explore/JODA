//
// Created by Nico Schäfer on 08/01/18.
//

#ifndef JODA_JOINFILECREATOR_H
#define JODA_JOINFILECREATOR_H
#include <string>
#include <mutex>
#include <unordered_map>
#include <rapidjson/document.h>
#include <joda/query/values/IValueProvider.h>
#include "../../../../storage/container/include/joda/container/JSONContainer.h"

typedef unsigned long JODA_JOINID ;
/**
 * The JoinManager is an interface for joining/grouping documents using different techniques.
 */
class JoinManager {
 public:
  /**
   * Initializes the name/identifier and IValueProvider of the JoinManager
   * @param name The name/identifier of the join
   * @param valProv  The IValueProvider to receive the join values from.
   */
  JoinManager(const std::string &name, std::unique_ptr<joda::query::IValueProvider>&& valProv);

  virtual ~JoinManager();
  /**
   * Joins a single document with the others
   * @param doc The RapidJsonDocument to join
   */
  virtual void join(const RapidJsonDocument& doc) = 0;

  /**
 * Joins a container
 * @param cont Container to join
 */
  virtual void join(JSONContainer& cont);

  /**
   * Returns the name/identifier of the join.
   * @return the name/identifier of the join
   */
  virtual const std::string &getName() const;
 protected:
  enum JoinVals {NOT_HASHABLE = 0,FALSE_VAL = 1, TRUE_VAL = 2, STARTNORMAL_VAL = 3};
  std::string name;
  std::unique_ptr<joda::query::IValueProvider> valProv;

};

#endif //JODA_JOINFILECREATOR_H
