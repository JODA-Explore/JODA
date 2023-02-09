#pragma once

#include <cstdint>
#include <joda/document/RapidJsonDocument.h>
#include "AdaptiveHistogram.h"
#include "environment/ManagedIndexData.h"

class NumberStats {
public:
    int64_t max = INT64_MIN;
    int64_t min = INT64_MAX;
};

class NumberIndex : public ManagedIndexData {
public:
    NumberIndex(std::vector<size_t> &documents, int64_t min, int64_t max, std::string &jsonPath);
    std::unique_ptr<const DocIndex> find(int64_t value, AdaptiveIndexQuery::COMPARISON_TYPE comparisonType, DataContext &dataContext);
    void CreateHistogramIndexWithValues(std::unique_ptr<std::vector<int64_t>> values, DataContext &dataContext);

    void used(DataContext &dataContext);
    size_t estimateWork(const AdaptiveIndexQuery &query);
    const bool isInitialized() const;
private:
    int64_t minValue;
    int64_t maxValue;
    std::vector<size_t>& relevantDocuments;

    std::unique_ptr<AdaptiveHistogram> histogram;

    void createHistogramIndexWithSplit(DataContext &dataContext);
};