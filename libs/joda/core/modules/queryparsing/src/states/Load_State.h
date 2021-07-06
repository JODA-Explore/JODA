//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_LOAD_STATE_H
#define JODA_LOAD_STATE_H

#define JODA_DEFAULT_LINE_SEPERATED false

#include <joda/parser/JSONFileDirectorySource.h>
#include <joda/parser/JSONFileSource.h>
#include <joda/storage/collection/StorageCollection.h>
#include "Query_State.h"
namespace joda::queryparsing::grammar {

enum Source_Value {
  NO_SOURCE,
  FILE_SOURCE,
  DIRECTORY_SOURCE,
  URL_SOURCE,
};

struct loadState {
  template <typename Input>
  inline loadState(const Input &in, queryState &qs) {}

  template <typename Input>
  inline void success(const Input &in, queryState &qs) {
    assert(sampleNum > 0 && sampleNum <= 1 && "Sample num has to be in (0,1)");
    qs.q->setLoad(loadVar);
    for (auto &&source : sources) {
      qs.q->addImportSource(std::move(source));
    }
    // Set LoadJoinManager
    qs.q->setLoadJoinManager(StorageCollection::getInstance().getOrStartJoin(
        std::move(groupvalprov)));
  }

  inline bool putValProv(std::unique_ptr<joda::query::IValueProvider> &&val) {
    assert(val != nullptr && "Should not pass nullptr");
    if (val == nullptr) return false;
    if (groupvalprov == nullptr) {
      groupvalprov = std::move(val);
      return true;
    }
    assert(false && "Should not be full");
    return false;
  }

  std::string loadVar;
  std::string loadFile;
  std::unique_ptr<joda::query::IValueProvider> groupvalprov;
  Source_Value source = NO_SOURCE;
  double sampleNum = 1;
  bool lineSeperated = JODA_DEFAULT_LINE_SEPERATED;
  std::vector<std::unique_ptr<docparsing::IImportSource>> sources;
};
}  // namespace joda::queryparsing::grammar
#endif  // JODA_LOAD_STATE_H
