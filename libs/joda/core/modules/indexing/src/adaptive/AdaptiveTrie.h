#pragma once

#include <joda/document/RapidJsonDocument.h>
#include "environment/ManagedIndexData.h"

class AdaptiveTrieNode {

public:
    AdaptiveTrieNode();
    AdaptiveTrieNode(char startingChar, int depth);

    int depthSize();
    int getDepth();
    void increaseDepthSize(char c);

    bool isVirtual = true;
    int depth;
    std::vector<char> chars;
    std::vector<size_t> exactMatchingDocs;
    std::vector<size_t> startMatchingDocs;

    std::vector<AdaptiveTrieNode> children;

    AdaptiveTrieNode* getChild(char c) {
        for (short i = 0; i < children.size(); ++i) {
            if (children[i].chars[0] == c) {
                return &children[i];
            }
        }

        return nullptr;
    }
};

class AdaptiveTrie : public ManagedIndexData {
public:
    AdaptiveTrie(std::string &jsonPath);

    void initialize(DataContext& dataContext, std::vector<size_t> &relevantDocuments, bool fullInit);
    void initializeByStep(std::string &value, int index);
    void finishStepInitialization(DataContext& dataContext);

    std::unique_ptr<const DocIndex> findEq(const std::string& cmpVal, DataContext& dataContext);
    std::unique_ptr<const DocIndex> findNotEq(const std::string& cmpVal, DataContext& dataContext, std::vector<size_t> &stringDocuments);
    std::unique_ptr<const DocIndex> findStartsWith(const std::string& cmpVal, DataContext& dataContext);
    std::unique_ptr<const DocIndex> findLex(const std::string& cmpVal, const AdaptiveIndexQuery::COMPARISON_TYPE& cmpType, DataContext& dataContext);

    unsigned long estimateWork(const AdaptiveIndexQuery &query);

    size_t static estimateInitMemory(size_t docCount);
    size_t static estimateExtractMemory(size_t valLength);
private:
    AdaptiveTrieNode* findMatchingNode(const std::string& cmpVal, DataContext& dataContext, bool startsWithSearch);
    void extract(AdaptiveTrieNode *node, DataContext& dataContext);
    void collectDocuments(AdaptiveTrieNode *currentNode, DocIndex &result);

    std::unique_ptr<AdaptiveTrieNode> root;
    RJPointer jsonPathPointer;
    size_t docCount;
};