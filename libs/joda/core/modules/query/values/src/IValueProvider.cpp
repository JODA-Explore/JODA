//
// Created by Nico Schäfer on 12/11/17.
//
#include "../include/joda/query/values/IValueProvider.h"
#include <glog/logging.h>
#include <joda/query/values/AtomProvider.h>
#include <joda/query/values/NullProvider.h>

#include "joda/misc/infix_iterator.h"

void joda::query::IValueProvider::replaceConstSubexpressions(
    std::unique_ptr<IValueProvider>& val) {
  if (val == nullptr) {
    return;
  }
  if (!val->params.empty() && val->isConst()) {
    RJMemoryPoolAlloc tmpAlloc;
    RapidJsonDocument tmpDoc;
    const RJValue* lhs;
    RJValue tmplhs;
    // Get Pointer to value
    if (val->isAtom()) {
      tmplhs = val->getAtomValue(tmpDoc, tmpAlloc);
      lhs = &tmplhs;
    } else {
      lhs = val->getValue(tmpDoc, tmpAlloc);
    }
    auto prev = val->toString();
    if (lhs == nullptr) {
      return;
    }
    if (lhs->IsNull()) {
      val = std::make_unique<NullProvider>();
    } else if (lhs->IsTrue()) {
      val = std::make_unique<BoolProvider>(true);
    } else if (lhs->IsFalse()) {
      val = std::make_unique<BoolProvider>(false);
    } else if (lhs->IsString()) {
      val = std::make_unique<StringProvider>(lhs->GetString());
    } else if (lhs->IsUint64()) {
      val = std::make_unique<UInt64Provider>(lhs->GetUint64());
    } else if (lhs->IsInt64()) {
      val = std::make_unique<Int64Provider>(lhs->GetInt64());
    } else if (lhs->IsNumber()) {
      val = std::make_unique<DoubleProvider>(lhs->GetDouble());
    } else if (lhs->IsObject()) {
      DLOG(WARNING) << "IsObject, but no Object Provider to replace";
    } else if (lhs->IsArray()) {
      DLOG(WARNING) << "IsArray, but no Array Provider to replace";
    }
    auto after = val->toString();
    DLOG(INFO) << "Replaced subpredicate \"" << prev << "\" with \"" << after
               << "\"";

  } else {
    for (auto& param : val->params) {
      replaceConstSubexpressions(param);
    }
  }
}

bool joda::query::IValueProvider::constBoolCheck(
    std::unique_ptr<IValueProvider>& val) {
  if (val->isBool() && val->isConst() &&
      !((val->toString() == JODA_TRUE_STRING) ||
        (val->toString() == JODA_FALSE_STRING))) {
    RJMemoryPoolAlloc alloc;
    auto bVal = val->getAtomValue(RapidJsonDocument(), alloc);
    if (bVal.IsBool()) {
      LOG(INFO) << "\"" + val->toString() << "\" evaluates to "
                << (bVal.GetBool() ? JODA_TRUE_STRING : JODA_FALSE_STRING)
                << " and was replaced";
      val = std::make_unique<BoolProvider>(bVal.GetBool());
      return true;
    }
  }
  return false;
}

joda::query::IValueProvider::IValueProvider(
    std::vector<std::unique_ptr<IValueProvider>>&& parameters)
    : params(std::move(parameters)) {}

joda::query::IValueProvider::IValueProvider()
    : IValueProvider(std::vector<std::unique_ptr<IValueProvider>>()) {}

std::string joda::query::IValueProvider::getParameterStringRepresentation()
    const {
  std::vector<std::string> pars;
  for (auto&& param : params) {
    pars.push_back(param->toString());
  }
  std::stringstream ss;
  std::move(std::begin(pars), std::end(pars),
            infix_ostream_iterator<std::string>(ss, ", "));
  return ss.str();
}

std::string joda::query::IValueProvider::toString() const {
  return this->getName() + "(" + getParameterStringRepresentation() + ")";
}

std::vector<std::unique_ptr<joda::query::IValueProvider>>
joda::query::IValueProvider::duplicateParameters() const {
  std::vector<std::unique_ptr<IValueProvider>> ret;
  for (auto&& param : params) {
    ret.push_back(param->duplicate());
  }
  return ret;
}

void joda::query::IValueProvider::checkParamSize(unsigned int expected) {
  if (params.size() != expected) {
    throw WrongParameterCountException(params.size(), expected, getName());
  }
}

void joda::query::IValueProvider::checkParamType(unsigned int i,
                                                 IValueType expected) {
  DCHECK_GE(params.size(), i) << "Checked for not existing parameter";
  if (!(params[i]->isAny() || params[i]->getReturnType() == expected)) {
    throw WrongParameterTypeException(i, expected, getName());
  }
}
