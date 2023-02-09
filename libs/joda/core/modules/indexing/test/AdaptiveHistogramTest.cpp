#include <gtest/gtest.h>
#include "../src/adaptive/UnionTreeNode.h"

class AdaptiveHistogramTest : public::testing::Test {
protected:
    AdaptiveHistogramTest()
            :   Test(),
                memoryManager(100 * 4,[this](std::string x, ManagedDataType dt) { ; }),
                dataContext(container) {

        dataContext.setMemoryManager(&memoryManager);

    }

    JSONContainer container;
    MemoryManager memoryManager;
    DataContext dataContext;

    std::string path1 = "/";
    std::string path2 = "/";
    std::vector<size_t> emptyDocs;
};

TEST_F(AdaptiveHistogramTest, TestSimple) {
    AdaptiveHistogram histogram(path1, 100);

    histogram.initialSplit(dataContext, emptyDocs, 0, 100);
}

TEST_F(AdaptiveHistogramTest, negMin) {
    int min = -49;
    int max = 149;

    int valueRange = std::abs(max - min);
    int bucketCount = 100; // 100 + 1 overflow bucket

    if (valueRange < 100) {
        bucketCount = valueRange;
    }
    if (valueRange < 1000) {
        bucketCount = 10;
    }

    int bucketSize = std::floor(valueRange / bucketCount);
    bucketCount++;

    std::vector<HistogramBucket> buckets;
    buckets.reserve(bucketCount);

    for (int i = 0; i < bucketCount; ++i) {
        buckets.push_back(HistogramBucket());
    }

    int v = -10;

    int bucket = (v - min) / bucketSize;
    buckets[bucket].values.push_back(v);
    buckets[bucket].docCount++;

    EXPECT_EQ(bucket, 2);
}

TEST_F(AdaptiveHistogramTest, posMin) {
    int min = 10;
    int max = 149;

    int valueRange = std::abs(max - min);
    int bucketCount = 100; // 100 + 1 overflow bucket

    if (valueRange < 100) {
        bucketCount = valueRange;
    }
    if (valueRange < 1000) {
        bucketCount = 10;
    }

    int bucketSize = std::floor(valueRange / bucketCount);
    bucketCount++;

    std::vector<HistogramBucket> buckets;
    buckets.reserve(bucketCount);

    for (int i = 0; i < bucketCount; ++i) {
        buckets.push_back(HistogramBucket());
    }

    int v = 36;

    int bucket = (v - min) / bucketSize;
    buckets[bucket].values.push_back(v);
    buckets[bucket].docCount++;

    EXPECT_EQ(bucket, 2);
}

TEST_F(AdaptiveHistogramTest, allNeg) {
    int min = -134;
    int max = -11;

    int valueRange = std::abs(max - min);
    int bucketCount = 100; // 100 + 1 overflow bucket

    if (valueRange < 100) {
        bucketCount = valueRange;
    }
    if (valueRange < 1000) {
        bucketCount = 10;
    }

    int bucketSize = std::floor(valueRange / bucketCount);
    bucketCount++;

    std::vector<HistogramBucket> buckets;
    buckets.reserve(bucketCount);

    for (int i = 0; i < bucketCount; ++i) {
        buckets.push_back(HistogramBucket());
    }

    int v = -100;

    int bucket = (v - min) / bucketSize;
    buckets[bucket].values.push_back(v);
    buckets[bucket].docCount++;

    EXPECT_EQ(bucket, 2);
}

TEST_F(AdaptiveHistogramTest, overflow) {
    int min = -49;
    int max = 149;

    int valueRange = std::abs(max - min);
    int bucketCount = 100; // 100 + 1 overflow bucket

    if (valueRange < 100) {
        bucketCount = valueRange;
    }
    if (valueRange < 1000) {
        bucketCount = 10;
    }

    int bucketSize = std::floor(valueRange / bucketCount);
    bucketCount++;

    std::vector<HistogramBucket> buckets;
    buckets.reserve(bucketCount);

    for (int i = 0; i < bucketCount; ++i) {
        buckets.push_back(HistogramBucket());
    }

    int v = 148;

    int bucket = (v - min) / bucketSize;
    buckets[bucket].values.push_back(v);
    buckets[bucket].docCount++;

    EXPECT_EQ(bucket, bucketCount - 1);
}

TEST_F(AdaptiveHistogramTest, valRange) {

    int min = 0;
    int max = 2;

    int valueRange = std::abs(max - min) + 1;

    EXPECT_EQ(3, valueRange);


    min = -20;
    max = 30;

    valueRange = std::abs(max - min) + 1;

    EXPECT_EQ(51, valueRange);
}