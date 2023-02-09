#pragma once

#include <cstdio>
#include <memory>
#include <vector>

class UnionTreeNodeDocumentHelper {
public:
    bool isWithinValidStringRange = false;
    bool isWithinValidNumberRange = false;
    bool isWithinValidObjectRange = false;
    bool isWithinValidTrueRange = false;
    bool isWithinValidFalseRange = false;
    bool isWithinValidNullRange = false;
    bool unsupportedDouble = false;

    std::unique_ptr<std::vector<size_t>> stringDocuments;
    std::unique_ptr<std::vector<size_t>> numberDocuments;
    std::unique_ptr<std::vector<size_t>> objectDocuments;
    std::unique_ptr<std::vector<size_t>> trueDocuments;
    std::unique_ptr<std::vector<size_t>> falseDocuments;
    std::unique_ptr<std::vector<size_t>> nullDocuments;

    // save int values for histogram creation
    std::unique_ptr<std::vector<int64_t>> numberValues;

    int64_t intMax = INT64_MIN;
    int64_t intMin = INT64_MAX;
    size_t totalDocs = 0;

    UnionTreeNodeDocumentHelper();
    void resetRange(size_t end);

    void checkString(size_t k);
    void checkNumber(size_t k);
    void checkObject(size_t k);
    void checkTrue(size_t k);
    void checkFalse(size_t k);
    void checkNull(size_t k);
};