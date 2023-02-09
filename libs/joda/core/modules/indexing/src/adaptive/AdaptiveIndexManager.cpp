#include "joda/indexing/AdaptiveIndexManager.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

AdaptiveIndexManager::AdaptiveIndexManager(const unsigned long containerSize)
: containerSize(containerSize) {
    memoryManager = std::make_unique<MemoryManager>(
            containerSize * 4,
            [this](std::string x, ManagedDataType dt) { removeElement(x, dt); } );

}

/**
 * Estimates the work for an index. Will only be called for SINGLE SUPPORTED index queries.
 * @param query
 * @return
 */
unsigned long AdaptiveIndexManager::estimateWork(const AdaptiveIndexQuery &query) {
    long estimatedWork;

    auto jsonStructureNodeEntry = reverseIndex.find(query.propertyPath);

    if (jsonStructureNodeEntry == reverseIndex.end() || !jsonStructureNodeEntry->second->isRelevantDocumentsCalculated()) {
        estimatedWork = estimateDocCount(query.propertyPath);
    } else if (jsonStructureNodeEntry->second->isRelevantDocumentsCalculated()) {
        estimatedWork = jsonStructureNodeEntry->second->estimateWork(query);
    } else {
        estimatedWork = containerSize - 1;
    }

    return estimatedWork;
}

std::shared_ptr<const DocIndex> AdaptiveIndexManager::executeQuery(const AdaptiveIndexQuery &query, DataContext &dataContext) {

    std::shared_ptr<const DocIndex> cachedResult = dataContext.getCacheResult(query);

    if (cachedResult) {
        return cachedResult;
    }

    dataContext.setMemoryManager(memoryManager.get());

    auto jsonStructureNodeEntry = reverseIndex.find(query.propertyPath);

    std::shared_ptr<UnionTreeNode> parentDocRangeNode;
    std::shared_ptr<UnionTreeNode> parentNode;

    if (jsonStructureNodeEntry == reverseIndex.end() || !jsonStructureNodeEntry->second->isRelevantDocumentsCalculated()) {
        parentDocRangeNode = buildAndGetParentDocRange(query.propertyPath, containerSize, dataContext);

        if (!parentDocRangeNode) {
            return nullptr;
        }

        auto parentIndexEntry = reverseIndex.find(query.propertyPath.substr(0, query.propertyPath.rfind("/")));
        if (parentIndexEntry != reverseIndex.end()) {
            parentNode = parentIndexEntry->second;
        } else {
            // cannot find parent node -> must be root
            parentNode = parentDocRangeNode;
        }

        jsonStructureNodeEntry = reverseIndex.find(query.propertyPath);
    }

    std::shared_ptr<const DocIndex> result;

    if (!jsonStructureNodeEntry->second->isRelevantDocumentsCalculated()) {
        result = jsonStructureNodeEntry->second->getDocumentsFirst(query, parentNode, parentDocRangeNode, dataContext);
        jsonStructureNodeEntry->second->nodeUsed(dataContext); // update used memory
    } else {
        result = jsonStructureNodeEntry->second->getDocumentsSecond(query, dataContext);
        jsonStructureNodeEntry->second->nodeUsed(dataContext); // update used memory
    }

    dataContext.previousQueryExecutionFinished();
    dataContext.addCacheResult(query, result);

    return result;
}

const std::shared_ptr<UnionTreeNode> AdaptiveIndexManager::buildAndGetParentDocRange(const std::string &propertyPath, int totalDocs, const DataContext &dataContext) {
    std::vector<std::string> splitPath;

    boost::split(splitPath, propertyPath, [](char c){return c == '/';});

    int neededMemory = splitPath.size() *
            (sizeof(UnionTreeNode) + sizeof(size_t))
            + propertyPath.size();
    if (!dataContext.getMemoryManager()->ensureMemory(neededMemory, nullptr)) {
        return nullptr;
    }

    std::string currentPath = "/"; // [0] is empty => root path

    auto entry = reverseIndex.find(currentPath);

    std::shared_ptr<UnionTreeNode> parentDocrangeNode;

    for (size_t j = 0; j < splitPath.size(); ++j) {

        // handle root path
        if (j == 0) {
            // create root node
            if (entry == reverseIndex.end()) {

                auto treeNode = std::make_shared<UnionTreeNode>("/");
                treeNode->setRootDocuments(totalDocs, dataContext);
                reverseIndex.insert({ "/", treeNode });
                treeNode->nodeUsed(dataContext);

                parentDocrangeNode = treeNode;
            } else {
                parentDocrangeNode = entry->second;
            }
            continue;
        }

        currentPath += splitPath[j];

        auto entry = reverseIndex.find(currentPath);

        if (entry == reverseIndex.end()) {
            auto treeNode = std::make_shared<UnionTreeNode>(currentPath);
            reverseIndex.insert({ currentPath, treeNode });
            treeNode->nodeUsed(dataContext);
        } else {
            if (entry->second->isRelevantDocumentsCalculated()) {
                parentDocrangeNode = entry->second;
            }
        }
        currentPath += "/";
    }

    return parentDocrangeNode;
}

const unsigned long AdaptiveIndexManager::estimateDocCount(const std::string &propertyPath) {

    unsigned long estimatedDocCount = containerSize;

    std::vector<std::string> splitPath;
    boost::split(splitPath, propertyPath, [](char c){return c == '/';});

    std::string currentPath = "/"; // [0] is empty => root path

    int j = 0;
    int maxDepth = 10;
    auto entry = reverseIndex.find(currentPath);

    while(entry != reverseIndex.end() && j < splitPath.size() && j < maxDepth) {
        estimatedDocCount = entry->second->getDocCount();

        currentPath += splitPath[j];
        entry = reverseIndex.find(currentPath);
        j++;
    }

    if (estimatedDocCount == containerSize && estimatedDocCount >= 10) {
        estimatedDocCount -= 10;
    }

    return estimatedDocCount;
}

void AdaptiveIndexManager::removeElement(std::string jsonPath, ManagedDataType dataType) {
    if (dataType == ManagedDataType::UnionTreeNodeData) {
        reverseIndex.erase(jsonPath);
    } else {
        auto unionTreeNode = reverseIndex.find(jsonPath);
        if (unionTreeNode != reverseIndex.end()) {
            unionTreeNode->second->removeElement(dataType);
        }
    }
}
