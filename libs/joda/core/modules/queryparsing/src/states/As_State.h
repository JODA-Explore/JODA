//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_AS_STATE_H
#define JODA_AS_STATE_H

#include <glog/logging.h>
#include <joda/query/project/IProjector.h>
#include <joda/query/project/ISetProjector.h>
#include <joda/query/values/IValueProvider.h>
#include "Query_State.h"
namespace joda::queryparsing::grammar {

enum projFun { NOPROJ, arrFlatProj, valProvProj, allCopyProj };

struct asState {
  template <typename Input>
  inline asState(const Input &in, queryState &qs) {
    fun = NOPROJ;
  }

  template <typename Input>
  inline void success(const Input &in, queryState &qs) {
    if (setprojs.empty() && projs.size() == 1 &&
        projs.front()->toString() == "*")
      return;  // If AS is only "*", then do not pass on projections
    for (auto &proj : projs) {
      qs.q->addProjection(std::move(proj));
    }
    for (auto &&proj : setprojs) {
      qs.q->addProjection(std::move(proj));
    }
  }

  inline bool putValProv(std::unique_ptr<joda::query::IValueProvider> &&val) {
    assert(val != nullptr && "Should not pass nullptr");
    if (val == nullptr) return false;
    if (valprov == nullptr) {
      valprov = std::move(val);
      return true;
    }
    DCHECK(false) << "Should not be full";
    return false;
  }

  std::string toPointer;
  projFun fun;
  std::unique_ptr<joda::query::IValueProvider> valprov;
  std::vector<std::unique_ptr<joda::query::IProjector>> projs;
  std::vector<std::unique_ptr<joda::query::ISetProjector>> setprojs;
};
}  // namespace joda::queryparsing::grammar
#endif  // JODA_AS_STATE_H
