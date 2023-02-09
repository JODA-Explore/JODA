#pragma once

#include <joda/container/JSONContainer.h>
#include "joda/document/RapidJsonDocument.h"
#include "joda/indexing/AdaptiveIndexQuery.h"
#include "AdaptiveTrie.h"
#include "NumberIndex.h"
#include "util/UnionTreeNodeDocumentHelper.h"

class UnionTreeNode : public ManagedIndexData {
public:
    UnionTreeNode(std::string jsonPath);

    std::unique_ptr<const DocIndex> getDocumentsFirst(
            const AdaptiveIndexQuery &query,
            std::shared_ptr<UnionTreeNode> parentNode,
            std::shared_ptr<UnionTreeNode> parentDocRangeNode,
            DataContext& dataContext);

    std::unique_ptr<const DocIndex> getDocumentsSecond(
            const AdaptiveIndexQuery &query,
            DataContext& dataContext);

    void setRootDocuments(size_t totalDocs, const DataContext &dataContext);

    const std::vector<size_t>& getObjectDocs() const;

    bool isRelevantDocumentsCalculated() const;

    size_t estimateWork(const AdaptiveIndexQuery &query);

    size_t getDocCount() const;

    void nodeUsed(const DataContext &dataContext);

    void removeElement(ManagedDataType dataType);
private:
    size_t totalDocs;

	// Documents
	bool relevantDocumentsCalculated = false;
    std::unique_ptr<std::vector<size_t>> stringDocuments;
    std::unique_ptr<std::vector<size_t>> numberDocuments;
    std::unique_ptr<std::vector<size_t>> objectDocuments;
    std::unique_ptr<std::vector<size_t>> trueDocuments;
    std::unique_ptr<std::vector<size_t>> falseDocuments;
    std::unique_ptr<std::vector<size_t>> nullDocuments;

    // Indexes
    std::unique_ptr<AdaptiveTrie> adaptiveTrie;
    std::unique_ptr<NumberIndex> numberIndex;

    void compress();

    bool handleValue(UnionTreeNodeDocumentHelper* nodeDocumentHelper, const RJValue *val, int k);

    void setResult(DocIndex &result, std::vector<size_t> *range);

    void createAdaptiveTrieIndex(DataContext &dataContext, bool fullInit);

    size_t getEstimatedFirstMemory(size_t docCount, bool knownNodeIsDirectParent, AdaptiveIndexQuery::CONTENT_TYPE contentType);
};