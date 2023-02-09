#pragma once

#include <joda/container/JSONContainer.h>
#include "joda/document/RapidJsonDocument.h"
#include "joda/indexing/AdaptiveIndexQuery.h"
#include "../../../src/adaptive/UnionTreeNode.h"

class AdaptiveIndexManager : public ContainerIndex {

public:
    AdaptiveIndexManager(const unsigned long containerSize);

	unsigned long estimateWork(const AdaptiveIndexQuery &query) override;

    std::shared_ptr<const DocIndex> executeQuery(const AdaptiveIndexQuery &query, DataContext &dataContext) override;
private:
    unsigned long containerSize;
	std::unordered_map<std::string, std::shared_ptr<UnionTreeNode>> reverseIndex;
	std::unique_ptr<MemoryManager> memoryManager;

    const std::shared_ptr<UnionTreeNode> buildAndGetParentDocRange(const std::string &propertyPath, int totalDocs, const DataContext &dataContext);

    const unsigned long estimateDocCount(const std::string &propertyPath);

    void removeElement(std::string jsonPath, ManagedDataType dataType);
};