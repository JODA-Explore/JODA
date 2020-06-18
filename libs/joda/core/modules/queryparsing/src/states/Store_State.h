//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_STORE_STATE_H
#define JODA_STORE_STATE_H

#include "Query_State.h"
#include <joda/storage/collection/StorageCollection.h>
namespace joda::queryparsing::grammar {
struct storeState {
  template<typename Input>
  inline storeState(const Input &in, queryState &qs) {
  }

  template<typename Input>
  inline void success(const Input &in, queryState &qs) {
    //Set LoadJoinManager
    qs.q->setStoreJoinManager(StorageCollection::getInstance().getOrStartJoin(std::move(groupvalprov)));
    qs.q->setExportDestination(std::move(exportDest));
  }

  inline bool putValProv(std::unique_ptr<joda::query::IValueProvider> &&val) {
    DCHECK(val != nullptr && "Should not pass nullptr");
    if (val == nullptr) return false;
    if (groupvalprov == nullptr) {
      groupvalprov = std::move(val);
      return true;
    }
    DCHECK(false) << "Should not be full";
    return false;
  }

  std::unique_ptr<joda::query::IValueProvider> groupvalprov;
  std::unique_ptr<IExportDestination> exportDest;
};
}
#endif //JODA_STORE_STATE_H
