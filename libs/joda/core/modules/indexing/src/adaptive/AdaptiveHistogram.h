#pragma once

#include <joda/container/JSONContainer.h>
#include "environment/DataContext.h"

class HistogramBucket {
public:
    HistogramBucket();

    size_t docCount = 0;
    std::vector<int64_t> values;
    std::vector<size_t> indexes;
    int64_t min;
    int64_t max;

    bool hasSub() { return buckets ? true : false; };

    void split(DataContext &dataContext);

    void findEqual(int64_t value, DocIndex &result, DataContext &dataContext);
    void findLess(int64_t value, DocIndex &result, DataContext &dataContext, bool include);
    void findMore(int64_t value, DocIndex &result, DataContext &dataContext, bool include);

    long estimateWork(const AdaptiveIndexQuery &query, int depth);
protected:
    int64_t bucketSize;
    std::unique_ptr<std::vector<HistogramBucket>> buckets;
    void setAll(DocIndex &result);
private:
    static int const max_effort_depth = 5;
};

class AdaptiveHistogram : public HistogramBucket {
public:
    AdaptiveHistogram(std::string &jsonPath, int splitCount);
    void initialSplit(DataContext& dataContext, std::vector<size_t> &relevantDocuments, int64_t min, int64_t max);

    void initialSplitWithValues(std::unique_ptr<std::vector<int64_t>> values, DataContext &dataContext, std::vector<size_t> &relevantDocuments, int64_t min, int64_t max);

    int getSplitCount() { return splitCount; };
    double getBucketThreshold() { return bucketThreshold; };

    size_t static estimateInitMemory(size_t docCount);
    size_t static estimateSplitMemory();
private:
    std::string &jsonPath;
    int splitCount;
    double bucketThreshold;
};
