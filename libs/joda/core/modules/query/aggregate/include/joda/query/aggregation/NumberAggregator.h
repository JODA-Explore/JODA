//
// Created by Nico Schäfer on 10/6/17.
//

#ifndef JODA_NUMBERAGGREGATOR_H
#define JODA_NUMBERAGGREGATOR_H

#include <joda/query/values/IValueProvider.h>
#include <cmath>
#include <unordered_map>
#include "IAggregator.h"

namespace joda::query {
struct ExampleNumberAggregatorClass {
  typedef double AggRep;
  static constexpr auto name = "EXAMPLE";

  inline static void aggregateFirst(AggRep &old, double val) { old = val; };

  inline static void aggregateRep(AggRep &old, AggRep &val) { old = val; };

  inline static void aggregate(AggRep &old, double val) { old = val; };

  inline static double getDouble(AggRep &val) { return val; };
};

/**
 * A class representing all simple single Aggregations of numbers
 * (min/max/count/sum/sub/...)
 * @tparam T
 */
template <class T>
class NumberAggregator : public IAggregator {
 public:
  NumberAggregator(const std::string &toPointer,
                   std::vector<std::unique_ptr<IValueProvider>> &&params)
      : IAggregator(toPointer, std::move(params)) {
    /*
     * check size
     */
    if (this->params.empty())
      throw WrongParameterCountException(params.size(), 1, getName());
    if (this->params.size() == 1) {
      enableArray = false;
    } else if (this->params.size() == 2) {
      checkParamType(1, IV_Bool);
      if (!this->params[1]->isConst())
        throw WrongParameterException("Parameter 1 has to be const bool");
      RJMemoryPoolAlloc alloc;
      enableArray =
          this->params[1]->getAtomValue(RapidJsonDocument(), alloc).GetBool();
    } else
      throw WrongParameterCountException(params.size(), 2, getName());

    auto type = this->params[0]->getReturnType();
    if (!(type == IV_Any || type == IV_Number ||
          (enableArray && type == IV_Array)))
      throw WrongParameterTypeException(0, IV_Number, getName());
  };

  void merge(IAggregator *other) override {
    auto *o = dynamic_cast<NumberAggregator<T> *>(other);
    assert(o != nullptr);
    assert(getName() == o->getName());
    assert(toPointer == o->toPointer);
    if (o->isAggregated) {
      if (this->isAggregated) {
        T::aggregateRep(this->val, o->val);
      } else {
        this->isAggregated = true;
        this->val = o->val;
      }
    }
  };

  RJValue terminate(RJMemoryPoolAlloc &alloc) override {
    RJValue val;
    if (isAggregated) {
      val.SetDouble(T::getDouble(this->val));
    }
    return val;
  };

  std::unique_ptr<IAggregator> duplicate() const override {
    return std::make_unique<NumberAggregator<T>>(toPointer,
                                                 duplicateParameters());
  };

  void accumulate(const RapidJsonDocument &json,
                  RJMemoryPoolAlloc &alloc) override {
    const RJValue *rjVal;
    RJValue val_;
    if (params[0]->isAtom()) {
      val_ = params[0]->getAtomValue(json, alloc);
      rjVal = &val_;
    } else {
      rjVal = params[0]->getValue(json, alloc);
    }
    if (rjVal != nullptr) {
      if (rjVal->IsNumber()) {
        if (isAggregated) {
          T::aggregate(this->val, rjVal->GetDouble());
        } else {
          isAggregated = true;
          T::aggregateFirst(this->val, rjVal->GetDouble());
        }
      }

      if (enableArray && rjVal->IsArray()) {
        for (const auto &item : rjVal->GetArray()) {
          if (item.IsNumber()) {
            if (isAggregated) {
              T::aggregate(this->val, rjVal->GetDouble());
            } else {
              isAggregated = true;
              T::aggregateFirst(this->val, rjVal->GetDouble());
            }
          }
        }
      }
    }
  };

  static constexpr auto getName_() { return T::name; }

  const std::string getName() const override { return getName_(); };

 protected:
  typename T::AggRep val{};
  bool isAggregated = false;
  bool enableArray = false;
};

/*
 * Aggregators
 */
struct MinAggregatorFunc {
  typedef double AggRep;
  static constexpr auto name = "MIN";

  inline static void aggregateFirst(AggRep &old, double val) { old = val; };

  inline static void aggregateRep(AggRep &old, AggRep &val) {
    old = std::min(old, val);
  };

  inline static void aggregate(AggRep &old, double val) {
    old = std::min(old, val);
  };

  inline static double getDouble(AggRep &val) { return val; };
};

typedef NumberAggregator<MinAggregatorFunc> MinAggregator;

struct MaxAggregatorFunc {
  typedef double AggRep;
  static constexpr auto name = "MAX";
  inline static void aggregateFirst(AggRep &old, double val) { old = val; };

  inline static void aggregateRep(AggRep &old, AggRep &val) {
    old = std::max(old, val);
  };

  inline static void aggregate(AggRep &old, double val) {
    old = std::max(old, val);
  };

  inline static double getDouble(AggRep &val) { return val; };
};

typedef NumberAggregator<MaxAggregatorFunc> MaxAggregator;

struct SumAggregatorFunc {
  typedef double AggRep;
  static constexpr auto name = "SUM";
  inline static void aggregateFirst(AggRep &old, double val) { old = val; };

  inline static void aggregateRep(AggRep &old, AggRep &val) { old += val; };

  inline static void aggregate(AggRep &old, double val) { old += val; };

  inline static double getDouble(AggRep &val) { return val; };
};

typedef NumberAggregator<SumAggregatorFunc> SumAggregator;

struct AverageAggregatorFunc {
  typedef std::pair<size_t, double> AggRep;
  static constexpr auto name = "AVG";
  inline static void aggregateFirst(AggRep &old, double val) {
    old = {1, val};
  };

  inline static void aggregateRep(AggRep &old, AggRep &val) {
    old.first += val.first;
    old.second += val.second;
  };

  inline static void aggregate(AggRep &old, double val) {
    old.first += 1;
    old.second += val;
  };

  inline static double getDouble(AggRep &val) {
    return val.second / val.first;
  };
};

typedef NumberAggregator<AverageAggregatorFunc> AverageAggregator;

template class NumberAggregator<MinAggregatorFunc>;

template class NumberAggregator<MaxAggregatorFunc>;

template class NumberAggregator<SumAggregatorFunc>;

template class NumberAggregator<AverageAggregatorFunc>;
}  // namespace joda::query
#endif  // JODA_NUMBERAGGREGATOR_H
