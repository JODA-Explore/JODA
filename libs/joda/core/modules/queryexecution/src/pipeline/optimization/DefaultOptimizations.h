#ifndef JODA_PIPELINE_DEFAULT_OPTIMIZATIONS_H
#define JODA_PIPELINE_DEFAULT_OPTIMIZATIONS_H

#include "OptimizationRule.h"
#include "SimpleOptimizationRule.h"
#include "QueryCombinationRule.h"
#include "FilterAggregationRule.h"

namespace joda::queryexecution::pipeline::optimization {

  struct DefaultOptimizations {
    static std::vector<std::unique_ptr<OptimizationRule>> getRules() {
      std::vector<std::unique_ptr<OptimizationRule>> rules;
      rules.emplace_back(std::make_unique<FileMapOptimization>()); // LSFileopener + LSFileReader => LFFileMapper
      rules.emplace_back(std::make_unique<QueryCombinationRule>()); // Store X + LOAD X => Buffer X
      rules.emplace_back(std::make_unique<FilterAggregationRule>()); // AS <empty> + Aggregate => FilterAggregate
      rules.emplace_back(std::make_unique<ChooseAggOptimization>()); // CHOOSE <X>  + FilterAggregate <>> = CHOOSEAGG <X>, <>>
      rules.emplace_back(std::make_unique<ChooseAsAggOptimization>()); // CHOOSE <X> + AS <Y> + AGG <Z> = CHOOSEASAGG <X>, <Y>, <Z>
      rules.emplace_back(std::make_unique<ChooseAsOptimization>()); // CHOOSE <X> + AS <Y> = CHOOSEAS <X>, <Y>
      return rules;
    }
  };

  }  // namespace joda::queryexecution::pipeline::optimization

#endif  // JODA_PIPELINE_DEFAULT_OPTIMIZATIONS_H