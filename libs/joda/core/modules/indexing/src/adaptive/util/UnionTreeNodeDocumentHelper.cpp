#include "UnionTreeNodeDocumentHelper.h"

UnionTreeNodeDocumentHelper::UnionTreeNodeDocumentHelper() {
    stringDocuments = std::make_unique<std::vector<size_t>>();
    numberDocuments = std::make_unique<std::vector<size_t>>();
    objectDocuments = std::make_unique<std::vector<size_t>>();
    trueDocuments = std::make_unique<std::vector<size_t>>();
    falseDocuments = std::make_unique<std::vector<size_t>>();
    nullDocuments = std::make_unique<std::vector<size_t>>();
}

void UnionTreeNodeDocumentHelper::resetRange(size_t end) {
    if (isWithinValidStringRange) {
        stringDocuments->push_back(end);
        isWithinValidStringRange = false;
    } else if (isWithinValidNumberRange) {
        numberDocuments->push_back(end);
        isWithinValidNumberRange = false;
    } else if (isWithinValidObjectRange) {
        objectDocuments->push_back(end);
        isWithinValidObjectRange = false;
    } else if (isWithinValidTrueRange) {
        trueDocuments->push_back(end);
        isWithinValidTrueRange = false;
    } else if (isWithinValidFalseRange) {
        falseDocuments->push_back(end);
        isWithinValidFalseRange = false;
    } else if (isWithinValidNullRange) {
        nullDocuments->push_back(end);
        isWithinValidNullRange = false;
    }
}

void UnionTreeNodeDocumentHelper::checkString(size_t k) {
    stringDocuments->push_back(k);
    resetRange(k-1);
    isWithinValidStringRange = true;
}

void UnionTreeNodeDocumentHelper::checkNumber(size_t k) {
    numberDocuments->push_back(k);
    resetRange(k-1);
    isWithinValidNumberRange = true;
}

void UnionTreeNodeDocumentHelper::checkObject(size_t k) {
    objectDocuments->push_back(k);
    resetRange(k-1);
    isWithinValidObjectRange = true;
}


void UnionTreeNodeDocumentHelper::checkTrue(size_t k) {
    trueDocuments->push_back(k);
    resetRange(k-1);
    isWithinValidTrueRange = true;
}

void UnionTreeNodeDocumentHelper::checkFalse(size_t k) {
    falseDocuments->push_back(k);
    resetRange(k-1);
    isWithinValidFalseRange = true;
}

void UnionTreeNodeDocumentHelper::checkNull(size_t k) {
    nullDocuments->push_back(k);
    resetRange(k-1);
    isWithinValidNullRange = true;
}