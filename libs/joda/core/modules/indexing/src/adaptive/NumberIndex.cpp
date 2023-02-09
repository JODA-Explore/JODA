#include "NumberIndex.h"

NumberIndex::NumberIndex(std::vector<size_t> &rd, int64_t min, int64_t max, std::string &jsonPath)
        : ManagedIndexData(jsonPath, ManagedDataType::NumberIndexData), minValue(min), maxValue(max), relevantDocuments(rd) {

    updateSize(sizeof(NumberIndex) + sizeof(size_t) * relevantDocuments.size());
}

void NumberIndex::CreateHistogramIndexWithValues(std::unique_ptr<std::vector<int64_t>> values, DataContext &dataContext) {
    if (!histogram) {
        dataContext.tempSizeTracker = 0;
        histogram = std::make_unique<AdaptiveHistogram>(jsonPath, 100);
        dataContext.setAdaptiveHistogram(histogram.get());

        dataContext.tempSizeTracker += sizeof(AdaptiveHistogram);
        histogram->initialSplitWithValues(std::move(values), dataContext, relevantDocuments, minValue, maxValue);

        updateLastUsed(dataContext.tempSizeTracker, dataContext);
    }
}

std::unique_ptr<const DocIndex> NumberIndex::find(int64_t value, AdaptiveIndexQuery::COMPARISON_TYPE comparisonType, DataContext &dataContext) {

    dataContext.tempSizeTracker = 0;

    if (!histogram) {
        createHistogramIndexWithSplit(dataContext);
    } else {
        dataContext.setAdaptiveHistogram(histogram.get());
    }

    DocIndex result(dataContext.size());

    switch (comparisonType) {
        case AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS:
        case AdaptiveIndexQuery::COMPARISON_TYPE::NOT_EQUALS:
            histogram->findEqual(value, result, dataContext);
            break;
        case AdaptiveIndexQuery::COMPARISON_TYPE::LT:
            histogram->findLess(value, result, dataContext, false);
            break;
        case AdaptiveIndexQuery::COMPARISON_TYPE::LTE:
            histogram->findLess(value, result, dataContext, true);
            break;
        case AdaptiveIndexQuery::COMPARISON_TYPE::GT:
            histogram->findMore(value, result, dataContext, false);
            break;
        case AdaptiveIndexQuery::COMPARISON_TYPE::GTE:
            histogram->findMore(value, result, dataContext, true);
            break;
    }

    if (comparisonType == AdaptiveIndexQuery::COMPARISON_TYPE::NOT_EQUALS) {
        result.flip();
    }

    updateLastUsed(dataContext.tempSizeTracker, dataContext);

    return std::make_unique<const DocIndex>(result);
}

size_t NumberIndex::estimateWork(const AdaptiveIndexQuery &query) {
    if (query.intVal > maxValue && (query.cmpType != AdaptiveIndexQuery::COMPARISON_TYPE::LT || query.cmpType != AdaptiveIndexQuery::COMPARISON_TYPE::LTE)) {
        return 0;
    } else if (query.intVal < minValue && (query.cmpType != AdaptiveIndexQuery::COMPARISON_TYPE::GT || query.cmpType != AdaptiveIndexQuery::COMPARISON_TYPE::GTE)) {
        return 0;
    } else {
        return histogram->estimateWork(query, 0);
    }
}

const bool NumberIndex::isInitialized() const {
    return histogram ? true : false;
}

void NumberIndex::used(DataContext &dataContext) {
    updateLastUsed(dataContext);
}


void NumberIndex::createHistogramIndexWithSplit(DataContext &dataContext) {
    if (!histogram) {
        histogram = std::make_unique<AdaptiveHistogram>(jsonPath, 100);
        dataContext.setAdaptiveHistogram(histogram.get());
        histogram->initialSplit(dataContext, relevantDocuments, minValue, maxValue);
    }
}
