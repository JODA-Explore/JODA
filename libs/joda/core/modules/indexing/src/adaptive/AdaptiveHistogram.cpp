#include <iostream>
#include "AdaptiveHistogram.h"

AdaptiveHistogram::AdaptiveHistogram(std::string &jsonPath, int splitCount) : jsonPath(jsonPath), splitCount(splitCount) {

}

HistogramBucket::HistogramBucket() : min(INT64_MAX), max(INT64_MIN) {
}

void AdaptiveHistogram::initialSplit(DataContext &dataContext, std::vector<size_t> &relevantDocuments, int64_t min, int64_t max) {
    dataContext.loadRange(relevantDocuments);

    this->min = min;
    this->max = max;

    int64_t valueRange = std::abs(max - min) + 1;
    int bucketCount = splitCount; // 100 + 1 overflow bucket; should be investigated what the optimal value is! Determines expected bucket size etc

    bucketSize = std::ceil(valueRange / (double)bucketCount);

    buckets = std::make_unique<std::vector<HistogramBucket>>();

    buckets->reserve(bucketCount);

    for (size_t i = 0; i < bucketCount; ++i) {
        buckets->push_back(HistogramBucket());
    }

    docCount = 0;

    RJPointer jsonPathPointer = RJPointer(jsonPath.c_str());

    for (size_t i = 0; i < relevantDocuments.size(); i += 2) {
        for (size_t k = relevantDocuments[i]; k <= relevantDocuments[i + 1]; ++k) {
            const auto *val = dataContext.at(k).Get(jsonPathPointer);
            int64_t value = val->GetInt64();
            int bucketNum = (value - min) / bucketSize;

            HistogramBucket &bucket = buckets->at(bucketNum);
            bucket.values.push_back(value);
            bucket.indexes.push_back(k);
            bucket.docCount++;

            if (value < bucket.min) {
                bucket.min = value;
            }

            if (value > bucket.max) {
                bucket.max = value;
            }

            docCount++;
        }
    }

    bucketThreshold = (docCount / bucketCount) * 1.1; // 10% more than

    dataContext.tempSizeTracker += docCount * 2 * sizeof(size_t) + bucketCount * sizeof(HistogramBucket);

    for (size_t i = 0; i < bucketCount; ++i) {
        if (buckets->at(i).docCount > bucketThreshold) {
            buckets->at(i).split(dataContext);
        }
    }
}

void AdaptiveHistogram::initialSplitWithValues(std::unique_ptr<std::vector<int64_t>> values, DataContext &dataContext,
                                               std::vector<size_t> &relevantDocuments, int64_t min, int64_t max) {

    this->min = min;
    this->max = max;

    int64_t valueRange = std::abs(max - min) + 1;
    int bucketCount = splitCount; // 100 + 1 overflow bucket; should be investigated what the optimal value is! Determines expected bucket size etc

    bucketSize = std::ceil(valueRange / (double)bucketCount);

    buckets = std::make_unique<std::vector<HistogramBucket>>();

    buckets->reserve(bucketCount);

    for (size_t i = 0; i < bucketCount; ++i) {
        buckets->push_back(HistogramBucket());
    }

    docCount = 0;

    std::vector<int64_t> &intValues = *values.get();

    for (size_t i = 0; i < relevantDocuments.size(); i += 2) {
        for (size_t k = relevantDocuments[i]; k <= relevantDocuments[i + 1]; ++k) {

            int64_t value = intValues[docCount]; // docCount == value index for next value
            int bucketNum = (value - min) / bucketSize;

            HistogramBucket &bucket = buckets->at(bucketNum);
            bucket.values.push_back(value);
            bucket.indexes.push_back(k);
            bucket.docCount++;

            if (value < bucket.min) {
                bucket.min = value;
            }

            if (value > bucket.max) {
                bucket.max = value;
            }

            docCount++;
        }
    }

    bucketThreshold = (docCount / bucketCount) * 1.1; // 10% more than

    dataContext.tempSizeTracker += docCount * 2 * sizeof(size_t) + bucketCount * sizeof(HistogramBucket);

    for (size_t i = 0; i < bucketCount; ++i) {
        if (buckets->at(i).docCount > bucketThreshold) {
            buckets->at(i).split(dataContext);
        }
    }
}

long HistogramBucket::estimateWork(const AdaptiveIndexQuery &query, int depth) {
    if (min > query.intVal || max < query.intVal) {
        return 0;
    }

    if (!hasSub()) {
        return docCount;
    }

    depth++;

    int bucketNum = (query.intVal - min) / bucketSize;
    HistogramBucket &bucket = buckets->at(bucketNum);

    if (bucket.docCount == 0 || bucket.min > query.intVal || bucket.max < query.intVal) {
        return 0;
    }

    if (bucket.hasSub() && depth < max_effort_depth) {
        return bucket.estimateWork(query, depth);
    } else {
        return bucket.docCount;
    }
}

void HistogramBucket::split(DataContext &dataContext) {

    int splitSize = dataContext.getAdaptiveHistogram()->getSplitCount();

    int64_t valueRange = std::abs(max - min) + 1;

    if (valueRange < splitSize) {
        splitSize = valueRange;
        bucketSize = 1;
    } else {
        bucketSize = std::ceil(valueRange / (double)splitSize);
    }

    buckets = std::make_unique<std::vector<HistogramBucket>>();
    buckets->reserve(splitSize);

    for (size_t i = 0; i < splitSize; ++i) {
        buckets->push_back(HistogramBucket());
    }

    for (size_t j = 0; j < indexes.size(); ++j) {

        int64_t value = values[j];
        int bucketNum = (value - min) / bucketSize;
        HistogramBucket &bucket = buckets->at(bucketNum);
        bucket.values.push_back(value);
        bucket.indexes.push_back(indexes[j]);
        bucket.docCount++;

        if (value < bucket.min) {
            bucket.min = value;
        }

        if (value > bucket.max) {
            bucket.max = value;
        }
    }

    dataContext.tempSizeTracker += splitSize * sizeof(HistogramBucket);

    // remove value / index copy in current bucket
    values.clear();
    values.shrink_to_fit();
    indexes.clear();
    indexes.shrink_to_fit();

    dataContext.setHistogramSplitted(true);
}

void HistogramBucket::findEqual(int64_t value, DocIndex &result, DataContext &dataContext) {
    if (min > value || max < value) {
        return;
    }

    int bucketNum = (value - min) / bucketSize;
    HistogramBucket &bucket = buckets->at(bucketNum);

    if (bucket.docCount == 0 || bucket.min > value || bucket.max < value) {
        return;
    }

    if (bucket.hasSub()) {
        bucket.findEqual(value, result, dataContext);
    } else {
        if (bucket.docCount > dataContext.getAdaptiveHistogram()->getBucketThreshold() && !dataContext.getHistogramSplitted() && bucket.min != bucket.max) {
            bucket.split(dataContext);
            bucket.findEqual(value, result, dataContext);
        } else {
            for (size_t i = 0; i < bucket.values.size(); ++i) {
                if (bucket.values[i] == value) {
                    result.set(bucket.indexes[i]);
                }
            }
        }
    }
}

void HistogramBucket::findLess(int64_t value, DocIndex &result, DataContext &dataContext, bool include) {
    if (value < min) {
        return;
    }

    int bucketNum = (value - min) / bucketSize;

    // target bucket needs to be checked
    HistogramBucket &bucket = buckets->at(bucketNum);
    if  (value >= bucket.min) {
        if (bucket.hasSub()) {
            bucket.findLess(value, result, dataContext, include);
        } else {
            if (bucket.docCount > dataContext.getAdaptiveHistogram()->getBucketThreshold() && !dataContext.getHistogramSplitted() && bucket.min != bucket.max) {
                bucket.split(dataContext);
                bucket.findLess(value, result, dataContext, include);
            } else {
                if (include) {
                    for (size_t i = 0; i < bucket.values.size(); ++i) {
                        if (bucket.values[i] <= value) {
                            result.set(bucket.indexes[i]);
                        }
                    }
                } else {
                    for (size_t i = 0; i < bucket.values.size(); ++i) {
                        if (bucket.values[i] < value) {
                            result.set(bucket.indexes[i]);
                        }
                    }
                }
            }
        }
    }

    // other buckets are definetly smaller and can just be added
    for (int b = (bucketNum - 1); b >= 0; --b) {
        HistogramBucket &bucket = buckets->at(b);

        if (bucket.hasSub()) {
            bucket.setAll(result);
        } else {
            for (size_t &i : bucket.indexes) {
                result.set(i);
            }
        }
    }
}

void HistogramBucket::findMore(int64_t value, DocIndex &result, DataContext &dataContext, bool include) {
    if (value > max) {
        return;
    }

    int bucketNum = (value - min) / bucketSize;

    // target bucket needs to be checked
    HistogramBucket &bucket = buckets->at(bucketNum);
    if  (value <= bucket.max) {
        if (bucket.hasSub()) {
            bucket.findMore(value, result, dataContext, include);
        } else {
            if (bucket.docCount > dataContext.getAdaptiveHistogram()->getBucketThreshold() && !dataContext.getHistogramSplitted() && bucket.min != bucket.max) {
                bucket.split(dataContext);
                bucket.findMore(value, result, dataContext, include);
            } else {
                if (include) {
                    for (size_t i = 0; i < bucket.values.size(); ++i) {
                        if (bucket.values[i] >= value) {
                            result.set(bucket.indexes[i]);
                        }
                    }
                } else {
                    for (size_t i = 0; i < bucket.values.size(); ++i) {
                        if (bucket.values[i] > value) {
                            result.set(bucket.indexes[i]);
                        }
                    }
                }
            }
        }
    }

    // other buckets are definetly smaller and can just be added
    for (size_t b = (bucketNum + 1); b < buckets->size(); ++b) {
        HistogramBucket &bucket = buckets->at(b);

        if (bucket.hasSub()) {
            bucket.setAll(result);
        } else {
            for (size_t &i : bucket.indexes) {
                result.set(i);
            }
        }
    }
}

void HistogramBucket::setAll(DocIndex &result) {
    if (hasSub()) {
        for(HistogramBucket &b : *buckets) {
            if (b.docCount == 0) {
                continue;
            }

            b.setAll(result);
        }
    } else {
        for (size_t &i : indexes) {
            result.set(i);
        }
    }
}

size_t AdaptiveHistogram::estimateInitMemory(size_t docCount) {
    return sizeof(AdaptiveHistogram) + 100 * sizeof(HistogramBucket) // Buckets
           + 90 * sizeof(HistogramBucket) // Splits max first level on init
           + docCount * 2 * sizeof(size_t); // Content: size_t value + size_t document index
}

size_t AdaptiveHistogram::estimateSplitMemory() {
    return 100 * sizeof(HistogramBucket);
}