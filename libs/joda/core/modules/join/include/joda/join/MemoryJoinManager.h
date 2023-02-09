//
// Created by Nico Schäfer on 02/04/18.
//

#ifndef JODA_MEMORYJOINMANAGER_H
#define JODA_MEMORYJOINMANAGER_H

#include <joda/storage/JSONStorage.h>
#include <unordered_map>
#include "JoinManager.h"
#include "MemoryJoinDoc.h"

/**
 * The MemoryJoinManager is responsible for joining/grouping documents using a
 * document in the memory
 */
class MemoryJoinManager : public JoinManager {
 public:
  /**
   * Initializes the MemoryJoinManager
   * @param name Name/Identifier of the join.
   * @param valProv The IValueProvider to receive the join values from.
   */
  MemoryJoinManager(const std::string &name,
                    std::unique_ptr<joda::query::IValueProvider> &&valProv);
  ~MemoryJoinManager() override;
  /**
   * Joins a single document with the others
   * @param doc The document to join
   */
  void join(const RapidJsonDocument &doc) override;
  const std::string &getName() const override;

  /**
   * Returns a full join container.
   * Has to be repeated until nullptr is returned.
   */
  std::shared_ptr<JSONContainer> loadOneJoinCont();

 protected:
  std::mutex mut;
  JODA_JOINID currID = STARTNORMAL_VAL;
  std::unordered_map<long, JODA_JOINID> lMap;         // Integer as long
  std::unordered_map<std::string, JODA_JOINID> sMap;  // String as String
  bool trueValCreated = false;
  bool falseValCreated = false;
  JODA_JOINID getJoinVal(const RJValue &val);
  size_t joinSize = 0;

  std::unordered_map<JODA_JOINID, std::unique_ptr<MemoryJoinDoc>> docs;
};

#endif  // JODA_MEMORYJOINMANAGER_H
