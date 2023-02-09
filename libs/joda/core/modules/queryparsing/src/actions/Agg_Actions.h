//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_AGG_ACTIONS_H
#define JODA_AGG_ACTIONS_H
#include <joda/query/aggregation/AttributeStatAggregator.h>
#include <joda/query/aggregation/CollectAggregator.h>
#include <joda/query/aggregation/CountAggregator.h>
#include <joda/query/aggregation/DistinctAggregator.h>
#include <joda/query/aggregation/GroupAggregator.h>
#include <joda/query/aggregation/HistogramAggregator.h>
#include <joda/query/aggregation/NumberAggregator.h>
#include <string>
#include <ranges>
#include <joda/extension/ModuleRegister.h>

#include "../grammar/Grammar.h"
#include "../states/States.h"

namespace joda::queryparsing::grammar {

template <>
struct aggExpAction<aggKW_DISTINCT> {
  static void apply0(aggState &state) {
    assert(state.aggfun == NOAGG);
    state.aggfun = DISTINCT;
  }
};
template <>
struct aggExpAction<aggKW_ATTSTAT> {
  static void apply0(aggState &state) {
    assert(state.aggfun == NOAGG);
    state.aggfun = ATTSTAT;
  }
};
template <>
struct aggExpAction<aggKW_AVG> {
  static void apply0(aggState &state) {
    assert(state.aggfun == NOAGG);
    state.aggfun = AVG;
  }
};
template <>
struct aggExpAction<aggKW_COUNT> {
  static void apply0(aggState &state) {
    assert(state.aggfun == NOAGG);
    state.aggfun = COUNT;
  }
};
template <>
struct aggExpAction<aggKW_SUM> {
  static void apply0(aggState &state) {
    assert(state.aggfun == NOAGG);
    state.aggfun = SUM;
  }
};
template <>
struct aggExpAction<aggKW_MIN> {
  static void apply0(aggState &state) {
    assert(state.aggfun == NOAGG);
    state.aggfun = MIN;
  }
};
template <>
struct aggExpAction<aggKW_COLLECT> {
  static void apply0(aggState &state) {
    assert(state.aggfun == NOAGG);
    state.aggfun = COLLECT;
  }
};
template <>
struct aggExpAction<aggKW_MAX> {
  static void apply0(aggState &state) {
    assert(state.aggfun == NOAGG);
    state.aggfun = MAX;
  }
};

template <>
struct aggExpAction<aggKW_HISTOGRAM> {
  static void apply0(aggState &state) {
    assert(state.aggfun == NOAGG);
    state.aggfun = HISTOGRAM;
  }
};

template <>
struct aggExpAction<aggKW_CUSTOM> {
  template <typename Input>
  static bool apply(const Input &in, aggState &state) {
    assert(state.aggfun == NOAGG);
    auto custom_funcs = joda::extension::ModuleRegister::getInstance().getAggFuncs();
    std::string custom_func = in.string();
    if (std::ranges::find(custom_funcs,custom_func) == custom_funcs.end()) {
      return false;
    }
    state.aggfun = CUSTOM;
    state.customAggName = custom_func;
    return true;
  }
};

template <>
struct aggExpAction<aggToPointer> {
  template <typename Input>
  static void apply(const Input &in, aggState &state) {
    assert(state.toPointer.empty());
    std::string pointer = in.string();
    state.toPointer = pointer.substr(1, pointer.size() - 2);
  }
};

template <>
struct aggExpAction<aggByExpr> {
  static void apply0(aggState &state) {
    DCHECK(!state.valprov.empty());
    DCHECK(state.groupedByValue == nullptr);
    auto beforeSize = state.valprov.size();
    state.groupedByValue = std::move(state.valprov.back());
    state.valprov.pop_back();
    DCHECK_EQ(beforeSize - 1, state.valprov.size());
    DCHECK(state.groupedByValue != nullptr);
  }
};

template <>
struct aggExpAction<aggAsIdent> {
  template <typename Input>
  static void apply(const Input &in, aggState &state) {
    DCHECK(state.groupAs.empty());
    std::string as = in.string();
    state.groupAs = as;
  }
};

template <>
struct aggExpAction<aggWindowSize> {
  template <typename Input>
  static bool apply(const Input &in, aggState &state) {
    std::string str = in.string();
    try {
      state.windowSize = std::stoull(str);
    } catch (std::exception &e) {
      LOG(ERROR) << "Could not parse number: " << str << ". " << e.what();
      return false;
    }
    return true;
  }
};

template <>
struct aggExpAction<aggWindowExp> {
  static void apply0(aggState &state) {
    state.window = true;
  }
};

template <>
struct aggExpAction<aggSingleExp> {
  template <typename Input>
  static void apply(const Input &in, aggState &state) {
    assert(state.aggfun != NOAGG);
    std::unique_ptr<joda::query::IAggregator> agg = nullptr;
    try {
      switch (state.aggfun) {
        case NOAGG:
          assert(false);
          break;
        case AVG:
          agg = std::make_unique<joda::query::AverageAggregator>(
              state.toPointer, std::move(state.valprov));
          break;
        case COUNT:
          agg = std::make_unique<joda::query::CountAggregator>(
              state.toPointer, std::move(state.valprov));
          break;
        case SUM:
          agg = std::make_unique<joda::query::SumAggregator>(
              state.toPointer, std::move(state.valprov));
          break;
        case ATTSTAT:
          agg = std::make_unique<joda::query::AttributeStatAggregator>(
              state.toPointer, std::move(state.valprov));
          break;
        case DISTINCT:
          agg = std::make_unique<joda::query::DistinctAggregator>(
              state.toPointer, std::move(state.valprov));
          break;
        case MIN:
          agg = std::make_unique<joda::query::MinAggregator>(
              state.toPointer, std::move(state.valprov));
          break;
        case MAX:
          agg = std::make_unique<joda::query::MaxAggregator>(
              state.toPointer, std::move(state.valprov));
          break;
        case COLLECT:
          agg = std::make_unique<joda::query::CollectAggregator>(
              state.toPointer, std::move(state.valprov));
          break;
        case HISTOGRAM:
          agg = std::make_unique<joda::query::HistogramAggregator>(
              state.toPointer, std::move(state.valprov));
        case CUSTOM:
          agg = joda::extension::ModuleRegister::getInstance().getAggFunc(state.customAggName,
              state.toPointer, std::move(state.valprov));
      }
    } catch (const std::exception &e) {
      throw tao::pegtl::parse_error(e.what(), in);
    }
    DCHECK(agg != nullptr);
    if (state.groupedByValue == nullptr) {  // Normal aggregation
      state.aggs.push_back(std::move(agg));
    } else {  // Group by aggregation
      auto groupAgg = std::make_unique<joda::query::GroupAggregator>(
          state.toPointer, std::move(state.groupedByValue), std::move(agg));
      if (!state.groupAs.empty()) {
        groupAgg->setGroupAs(state.groupAs);
        state.groupAs.clear();
      }
      state.aggs.push_back(std::move(groupAgg));
    }
    DCHECK(agg == nullptr);
    DCHECK(state.groupedByValue == nullptr);
    state.valprov.clear();
    state.toPointer.clear();
    state.aggfun = NOAGG;
    state.customAggName.clear();
    assert(state.valprov.empty());
    assert(state.toPointer.empty());
    assert(state.aggfun == NOAGG);
  }
};
}  // namespace joda::queryparsing::grammar
#endif  // JODA_AGG_ACTIONS_H
