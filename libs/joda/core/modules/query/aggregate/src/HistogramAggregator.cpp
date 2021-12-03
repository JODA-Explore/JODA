
#include "../include/joda/query/aggregation/HistogramAggregator.h"

#include <rapidjson/document.h>
#include <rapidjson/fwd.h>

RJValue joda::query::HistogramAggregator::terminate(RJMemoryPoolAlloc& alloc) {
  RJValue histogram(rapidjson::kArrayType);
  for (auto&& x :
       boost::histogram::indexed(hist, boost::histogram::coverage::all)) {
    RJValue bucket(rapidjson::kObjectType);
    auto val = (size_t)*x;

    auto lower = x.bin().lower();
    if (lower >
        -std::numeric_limits<double>::infinity())  // Lower overflow bucket
      bucket.AddMember("lower", lower, alloc);
    else if (val == 0) {  // Skip empty overflow bucket
      continue;
    }

    auto upper = x.bin().upper();
    if (upper <
        std::numeric_limits<double>::infinity())  // Upper overflow bucket
      bucket.AddMember("upper", upper, alloc);
    else if (val == 0) {  // Skip empty overflow bucket
      continue;
    }

    bucket.AddMember("value", val, alloc);
    histogram.PushBack(bucket, alloc);
  }
  return histogram;
}

std::unique_ptr<joda::query::IAggregator>
joda::query::HistogramAggregator::duplicate() const {
  return std::make_unique<joda::query::HistogramAggregator>(
      toPointer, duplicateParameters());
}

joda::query::HistogramAggregator::HistogramAggregator(
    const std::string& toPointer,
    std::vector<std::unique_ptr<IValueProvider>>&& params)
    : IAggregator(toPointer, std::move(params)) {
  checkMinParamSize(4);
  checkParamType(1, IV_Number);  // Bins
  if (!this->params[1]->isConst())
    throw WrongParameterException("Parameter 1 has to be const bool");
  checkParamType(2, IV_Number);  // Min
  if (!this->params[2]->isConst())
    throw WrongParameterException("Parameter 2 has to be const bool");
  checkParamType(3, IV_Number);  // Max
  if (!this->params[3]->isConst())
    throw WrongParameterException("Parameter 3 has to be const bool");
  RJMemoryPoolAlloc alloc;
  RapidJsonDocument doc;
  auto bins = this->params[1]->getAtomValue(doc, alloc).GetInt64();
  auto min = this->params[2]->getAtomValue(doc, alloc).GetDouble();
  auto max = this->params[3]->getAtomValue(doc, alloc).GetDouble();
  hist = boost::histogram::make_histogram(
      boost::histogram::axis::regular<>(bins, min, max));
}

void joda::query::HistogramAggregator::accumulate(const RapidJsonDocument& json,
                                                  RJMemoryPoolAlloc& alloc) {
  const RJValue* val;
  RJValue val_;
  if (params[0]->isAtom()) {
    val_ = params[0]->getAtomValue(json, alloc);
    val = &val_;
  } else {
    val = params[0]->getValue(json, alloc);
  }
  if (val != nullptr) {
    if (val->IsNumber()) {
      if (val->IsInt64())
        hist(val->GetInt64());
      else
        hist(val->GetDouble());
    }
  }
}

const std::string joda::query::HistogramAggregator::getName() const {
  return getName_();
}

void joda::query::HistogramAggregator::merge(IAggregator* other) {
  auto* o = dynamic_cast<joda::query::HistogramAggregator*>(other);
  CHECK_NOTNULL(o);
  DCHECK_EQ(o->hist.size(), hist.size());
  auto at =
      boost::histogram::indexed(hist, boost::histogram::coverage::all).begin();
  // Iterate base histogram and other histogram simultaneously
  for (const auto& x :
       boost::histogram::indexed(o->hist, boost::histogram::coverage::all)) {
    // Bins should be the same
    DCHECK_EQ(x.index(), at->index());
    DCHECK_EQ(x.bin().lower(), at->bin().lower());
    DCHECK_EQ(x.bin().upper(), at->bin().upper());

    // Merge bins
    at->get() += *x;

    at++;  // Advance base Pointer
  }
}