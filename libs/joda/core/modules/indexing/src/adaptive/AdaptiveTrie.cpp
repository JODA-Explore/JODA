#include "AdaptiveTrie.h"
#include <iostream>

AdaptiveTrie::AdaptiveTrie(std::string &jp) : ManagedIndexData(jp, ManagedDataType::AdaptiveTrieData) {
    docCount = 0;
    updateSize(sizeof(AdaptiveTrie));
}

AdaptiveTrieNode::AdaptiveTrieNode(char startingChar, int depth) : depth(depth) {
    isVirtual = true;
    chars.push_back(startingChar);
}

void AdaptiveTrie::initialize(DataContext& dataContext, std::vector<size_t> &relevantDocuments, bool fullInit) {
    dataContext.tempSizeTracker = 0;

    root = std::make_unique<AdaptiveTrieNode>();
    dataContext.tempSizeTracker += sizeof(AdaptiveTrieNode);

    jsonPathPointer = RJPointer(jsonPath.c_str());

    if (fullInit) {
        root->startMatchingDocs = relevantDocuments;
        extract(root.get(), dataContext);
        root->startMatchingDocs.clear();

        dataContext.tempSizeTracker += docCount * sizeof(size_t); // all doc IDs + AdaptiveTrie fields + root node
    } else {
        root->isVirtual = false; // will be extracted step by step
    }

    updateLastUsed(dataContext.tempSizeTracker, dataContext);
}

void AdaptiveTrie::initializeByStep(std::string &value, int k) {
    //std::unordered_map<char, std::unique_ptr<AdaptiveTrieNode>> &children = root->children;

    int depth = 0;
    int nextDepth = 1;

    auto childNode = root->getChild(value.at(depth));

    if (childNode == nullptr) {
        root->children.push_back(AdaptiveTrieNode(value.at(0), nextDepth));
        childNode = &root->children[root->children.size() - 1];
    }

    if (value.length() == nextDepth) {
        childNode->exactMatchingDocs.push_back(k);
    } else {
        childNode->startMatchingDocs.push_back(k);
    }


    /*if (children.find(value.at(depth)) == children.end()) {
        children.insert(std::make_pair(value.at(depth),
                                       std::make_unique<AdaptiveTrieNode>(value.at(depth), nextDepth)));
    }

    if (value.length() == nextDepth) {
        children.find(value.at(depth))->second->exactMatchingDocs.push_back(k);
    } else {
        children.find(value.at(depth))->second->startMatchingDocs.push_back(k);
    }*/

    docCount++;
}

void AdaptiveTrie::finishStepInitialization(DataContext& dataContext) {
    dataContext.tempSizeTracker = 0;
    dataContext.tempSizeTracker += root->children.size() * (sizeof(char) + sizeof(size_t) + sizeof(AdaptiveTrieNode)) // char + isVirtualBool + depth
                + docCount * sizeof(size_t); // all doc IDs + AdaptiveTrie fields + root node
    updateLastUsed(dataContext.tempSizeTracker, dataContext);
}

std::unique_ptr<const DocIndex> AdaptiveTrie::findEq(const std::string &cmpVal, DataContext& dataContext) {
    DocIndex result(dataContext.size());

    dataContext.tempSizeTracker = 0;
    AdaptiveTrieNode *currentNode = findMatchingNode(cmpVal, dataContext, false);
    updateLastUsed(dataContext.tempSizeTracker, dataContext);

    if (currentNode != nullptr) {
        for (size_t i = 0; i < currentNode->exactMatchingDocs.size(); ++i) {
            result.set(currentNode->exactMatchingDocs[i]);
        }
    }

    return std::make_unique<const DocIndex>(std::move(result));
}

std::unique_ptr<const DocIndex> AdaptiveTrie::findNotEq(const std::string &cmpVal, DataContext& dataContext, std::vector<size_t> &stringDocuments) {
    DocIndex result(dataContext.size());

    dataContext.tempSizeTracker = 0;
    AdaptiveTrieNode *currentNode = findMatchingNode(cmpVal, dataContext, false);
    updateLastUsed(dataContext.tempSizeTracker, dataContext);

    if (currentNode != nullptr) {
        for (size_t i = 0; i < currentNode->exactMatchingDocs.size(); ++i) {
            result.set(currentNode->exactMatchingDocs[i]);
        }
    }

    result.flip();

    return std::make_unique<const DocIndex>(std::move(result));
}


std::unique_ptr<const DocIndex> AdaptiveTrie::findStartsWith(const std::string &cmpVal, DataContext& dataContext) {
    DocIndex result(dataContext.size());

    dataContext.tempSizeTracker = 0;
    AdaptiveTrieNode *currentNode = findMatchingNode(cmpVal, dataContext, true);
    updateLastUsed(dataContext.tempSizeTracker, dataContext);

    if (currentNode == nullptr || (currentNode->isVirtual && currentNode->startMatchingDocs.size() == 0)) {
        return std::make_unique<const DocIndex>(std::move(result));
    } else {
        collectDocuments(currentNode, result);
    }

    return std::make_unique<const DocIndex>(std::move(result));
}

std::unique_ptr<const DocIndex>
AdaptiveTrie::findLex(const std::string &cmpVal, const AdaptiveIndexQuery::COMPARISON_TYPE& cmpType, DataContext &dataContext) {
    DocIndex result(dataContext.size());

    dataContext.tempSizeTracker = 0;

    int depthIndex = 0;

    AdaptiveTrieNode *currentNode = root.get();

    while (depthIndex < cmpVal.length()) {
        if (currentNode->isVirtual) {
            extract(currentNode, dataContext);

            currentNode->startMatchingDocs.clear();
            currentNode->startMatchingDocs.shrink_to_fit();
        }

        // Case: compressed node
        if (currentNode->depthSize() > 1) {
            for (size_t i = 1; i < currentNode->depthSize(); ++i) {

                if ((cmpVal.size() - 1) < depthIndex) {

                    // cmpValue is shorter than chain, but until then everything is equal => content is greater => if "greater" is needed, add all

                    if (cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GTE || cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GT) {
                        collectDocuments(currentNode, result);
                        return std::make_unique<const DocIndex>(std::move(result)); // the end
                    }
                }

                if (currentNode->chars[i] != cmpVal[depthIndex]) {

                    if ((cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LTE || cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LT) && currentNode->chars[i] < cmpVal[depthIndex]) {
                        // all elements on currentnode continue the path of a smaller char -> add if smaller is wanted
                        collectDocuments(currentNode, result);
                    } else if ((cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GTE || cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GT) && currentNode->chars[i] > cmpVal[depthIndex]) {
                        // all elements on currentnode continue the path of a greater char -> add if greater is wanted
                        collectDocuments(currentNode, result);
                    }

                    return std::make_unique<const DocIndex>(std::move(result));
                }

                depthIndex++;
            }

            depthIndex--;
        }

        // You made it this far! cmpValue and all chars seem to be the same

        // any exact matches to add?
        if ((cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LTE || cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GTE)
            && ((cmpVal.size() - 1) == depthIndex)) {
            // current value matches EXACTLY cmpValue -> add if >= or <=
            for (size_t i = 0; i < currentNode->exactMatchingDocs.size(); ++i) {
                result.set(currentNode->exactMatchingDocs[i]);
            }
        } else if ((cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LTE || cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LT) && ((cmpVal.size() - 1) > depthIndex)) {
            // current value matches up until now to stuff and cmpValue is bigger -> all exact values are shorter than cmpVal -> add if LT
            for (size_t i = 0; i < currentNode->exactMatchingDocs.size(); ++i) {
                result.set(currentNode->exactMatchingDocs[i]);
            }
        }


        // for each child in extracted node WHICH IS NOT EQUAL (equal cases need to be looked at in the next loop): if lex condition fulfilled -> add, else ignore
        for (auto &child : currentNode->children) {
            if (child.chars[0] != cmpVal[depthIndex]) {
                if (cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LTE ||
                    cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LT) {

                    if (child.chars[0] < cmpVal[depthIndex]) {
                        collectDocuments(&child, result);
                    }
                } else if (cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GTE ||
                           cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GT) {

                    if (child.chars[0] > cmpVal[depthIndex]) {
                        collectDocuments(&child, result);
                    }
                }
            }
        }


        /*for(auto &child : currentNode->children) {
            if (child.first != cmpVal[depthIndex]) {

                if (cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LTE || cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LT) {

                    if (child.first < cmpVal[depthIndex]) {
                        collectDocuments(child.second.get(), result);
                    }
                } else if (cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GTE || cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GT) {

                    if (child.first > cmpVal[depthIndex]) {
                        collectDocuments(child.second.get(), result);
                    }
                }

            }
        }*/

        // check additional charge of possibly compressed path, no need to check
        /*for (size_t i = 1; i < currentNode->depthSize(); ++i) {
            if (currentNode->chars[i] != cmpVal[depthIndex]) {

                if (cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LTE || cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LT) {

                    if (currentNode->chars[i] < cmpVal[depthIndex]) {
                        collectDocuments(currentNode, result);
                    }
                } else if (cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GTE || cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GT) {

                    if (currentNode->chars[i] > cmpVal[depthIndex]) {
                        collectDocuments(currentNode, result);
                    }
                }

                //return nullptr;
            }

            depthIndex++;
        }*/

        // yea...make it right
        if (depthIndex >= cmpVal.length()) {
            break;
        }


        auto charNodeEntry = currentNode->getChild(cmpVal.at(depthIndex));

        if (charNodeEntry == nullptr) {
            break;
        }

        currentNode = charNodeEntry;

        depthIndex++;
    }

    updateLastUsed(dataContext.tempSizeTracker, dataContext);

    return std::make_unique<const DocIndex>(std::move(result));
}

void AdaptiveTrie::collectDocuments(AdaptiveTrieNode *currentNode, DocIndex &result) {
    for (size_t i = 0; i < currentNode->exactMatchingDocs.size(); ++i) {
        result.set(currentNode->exactMatchingDocs[i]);
    }

    if (!currentNode->startMatchingDocs.empty()) {
        for (size_t i = 0; i < currentNode->startMatchingDocs.size(); ++i) {
            result.set(currentNode->startMatchingDocs[i]);
        }
    } else {
        for(auto &child : currentNode->children) {
            collectDocuments(&child, result);
        }
    }
}

AdaptiveTrieNode* AdaptiveTrie::findMatchingNode(const std::string &cmpVal, DataContext& dataContext, bool startsWithSearch) {
    int depthIndex = 0;

    AdaptiveTrieNode *currentNode = root.get();

    while (depthIndex < cmpVal.length()) {
        if (currentNode->isVirtual) {
            extract(currentNode, dataContext);

            currentNode->startMatchingDocs.clear();
            currentNode->startMatchingDocs.shrink_to_fit();
        }

        // check additional charge of possibly compressed path, no need to check
        for (size_t i = 1; i < currentNode->depthSize(); ++i) {
            if (currentNode->chars[i] != cmpVal[depthIndex]) {
                if (depthIndex >= cmpVal.length() && startsWithSearch) {
                    // Case: due to compression there's no node that matches the STARTSWITH cmpVal exactly
                    // -> return this node as it contains all documents which are more than the cmpVal
                    return currentNode;
                } else {
                    // Case 2: This is not a node with a path that matches all characters in cmpVal
                    // -> a node with exact matches or matches starting with cmpVal does not exist
                    return nullptr;
                }
            }

            depthIndex++;
        }

        if (depthIndex >= cmpVal.length()) {
            break;
        }

        auto charNodeEntry = currentNode->getChild(cmpVal.at(depthIndex));

        if (charNodeEntry == nullptr) {
            return nullptr;
        }

        currentNode = charNodeEntry;

        depthIndex++;
    }

    return currentNode;
}

void AdaptiveTrie::extract(AdaptiveTrieNode *node, DataContext& dataContext) {

    if (node->startMatchingDocs.size() == 0) {
        node->isVirtual = false;
        return;
    }

    bool multiExtract;

    int depth = node->getDepth();

    do {
        int nextDepth = depth + 1;

        //std::unordered_map<char, std::unique_ptr<AdaptiveTrieNode>> &children = node->children;
        std::vector<AdaptiveTrieNode> &children = node->children;

        // depth == 0 is initialization root case only!
        if (depth == 0) {
            dataContext.loadRange(node->startMatchingDocs);
            children.reserve(250);

            for (size_t i = 0; i < node->startMatchingDocs.size(); i += 2) {
                for (size_t k = node->startMatchingDocs[i]; k <= node->startMatchingDocs[i + 1]; ++k) {
                    const auto *val = dataContext.at(k).Get(jsonPathPointer);

                    if (!val->IsString()) {
                        std::cout << "not a string but is " << val->GetType() << std::endl;
                    }

                    std::string value = val->GetString();

                    auto childNode = node->getChild(value.at(depth));

                    if (childNode == nullptr) {
                        children.push_back(AdaptiveTrieNode(value.at(depth), nextDepth));
                        childNode = &children[children.size() - 1];
                    }

                    if (value.length() == nextDepth) {
                        childNode->exactMatchingDocs.push_back(k);
                    } else {
                        childNode->startMatchingDocs.push_back(k);
                    }

                    /*if (children.find(value.at(depth)) == children.end()) {
                        children.insert(std::make_pair(value.at(depth), std::make_unique<AdaptiveTrieNode>(value.at(depth), nextDepth)));

                    }

                    if (value.length() == nextDepth) {
                        children.find(value.at(depth))->second->exactMatchingDocs.push_back(k);
                    } else {
                        children.find(value.at(depth))->second->startMatchingDocs.push_back(k);
                    }*/

                    docCount++;
                }
            }

            children.shrink_to_fit();

            dataContext.tempSizeTracker += children.size() * ((sizeof(char) + sizeof(size_t) + sizeof(AdaptiveTrieNode))); // char + isVirtualBool + depth

        } else {
            dataContext.load(node->startMatchingDocs);

            if (children.size() == 0) {
                    children.reserve(16);
            }

            bool useCache = node->startMatchingDocs.size() != 0 && node->startMatchingDocs.size() > dataContext.size() / 5;

            if (useCache) {

                if (dataContext.trieValueCache.size() == 0) {

                    if (node->startMatchingDocs.size() != 0) {
                        dataContext.trieValueCacheIndex.resize(dataContext.size());
                        dataContext.trieValueCache.resize(node->startMatchingDocs.size());
                    }

                    for (size_t i = 0; i < node->startMatchingDocs.size(); ++i) {
                        size_t docIndex = node->startMatchingDocs[i];
                        const auto *val = dataContext.at(docIndex).Get(jsonPathPointer);

                        dataContext.trieValueCache[i] = val->GetString();
                        dataContext.trieValueCacheIndex[docIndex] = i;

                        auto &value = dataContext.trieValueCache[i];

                        auto childNode = node->getChild(value.at(depth));

                        if (childNode == nullptr) {
                            children.push_back(AdaptiveTrieNode(value.at(depth), nextDepth));
                            childNode = &children[children.size() - 1];
                        }

                        if (value.length() == nextDepth) {
                            childNode->exactMatchingDocs.push_back(docIndex);
                        } else {
                            childNode->startMatchingDocs.push_back(docIndex);
                        }
                    }
                } else {
                    for (size_t i = 0; i < node->startMatchingDocs.size(); ++i) {
                        size_t docIndex = node->startMatchingDocs[i];

                        std::string &value = dataContext.trieValueCache[dataContext.trieValueCacheIndex[docIndex]];

                        auto childNode = node->getChild(value.at(depth));

                        if (childNode == nullptr) {
                            children.push_back(AdaptiveTrieNode(value.at(depth), nextDepth));
                            childNode = &children[children.size() - 1];
                        }

                        if (value.length() == nextDepth) {
                            childNode->exactMatchingDocs.push_back(docIndex);
                        } else {
                            childNode->startMatchingDocs.push_back(docIndex);
                        }
                    }
                }
            } else {
                for (size_t i = 0; i < node->startMatchingDocs.size(); ++i) {
                    size_t docIndex = node->startMatchingDocs[i];
                    const auto *val = dataContext.at(docIndex).Get(jsonPathPointer);

                    std::string value = val->GetString();

                    auto childNode = node->getChild(value.at(depth));

                    if (childNode == nullptr) {
                        children.push_back(AdaptiveTrieNode(value.at(depth), nextDepth));
                        childNode = &children[children.size() - 1];
                    }

                    if (value.length() == nextDepth) {
                        childNode->exactMatchingDocs.push_back(docIndex);
                    } else {
                        childNode->startMatchingDocs.push_back(docIndex);
                    }
                }
            }

            /*for (size_t &docIndex : node->startMatchingDocs) {
                const auto *val = dataContext.at(docIndex).Get(jsonPathPointer);

                std::string value = val->GetString();

                auto childNode = node->getChild(value.at(depth));

                if (childNode == nullptr) {
                    children.push_back(AdaptiveTrieNode(value.at(depth), nextDepth));
                    childNode = &children[children.size() - 1];
                }

                if (value.length() == nextDepth) {
                    childNode->exactMatchingDocs.push_back(docIndex);
                } else {
                    childNode->startMatchingDocs.push_back(docIndex);
                }
            }*/

            children.shrink_to_fit();

            dataContext.tempSizeTracker += children.size() * ((sizeof(char) + sizeof(size_t) + sizeof(AdaptiveTrieNode))); // char + isVirtualBool + depth
        }

        // Compress into current node if there's only one child
        if (children.size() == 1 && node->exactMatchingDocs.size() == 0) {
            node->increaseDepthSize(children[0].chars[0]);

            // if the one child node has exact matches, transfer these matches to own node
            node->exactMatchingDocs = children[0].exactMatchingDocs;
            node->startMatchingDocs = children[0].startMatchingDocs;

            children.clear();
            depth++;
            multiExtract = true;

            // remove the 1 added child, but not the extra char that is saved with increaseDepthSize
            dataContext.tempSizeTracker -= (sizeof(char) + sizeof(size_t) + sizeof(AdaptiveTrieNode)); // map char + isVirtualBool + depth
            dataContext.tempSizeTracker += sizeof(char); // char in list

        } else {
            multiExtract = false;
        }
    } while (multiExtract);

    node->isVirtual = false;
}

/**
 * Tries to estimate effort by getting the first trie child nodes. If they're not available, a fixed factor is used.
 * @param query
 * @return
 */
unsigned long AdaptiveTrie::estimateWork(const AdaptiveIndexQuery &query) {
    if (query.strValue.size() >= 1) {

        auto charNodeEntry = root->getChild(query.strValue.at(0));

        if (charNodeEntry == nullptr) {
            return root->startMatchingDocs.size() / 2;
        } else {
            if (charNodeEntry->children.size() == 0) {
                return query.strValue.size() == 1 ? charNodeEntry->exactMatchingDocs.size() : charNodeEntry->startMatchingDocs.size();
            } else {
                return docCount / 4;
            }
        }
    } else {
        return 1;
    }
}

int AdaptiveTrieNode::depthSize() {
    return chars.size();
}

int AdaptiveTrieNode::getDepth() {
    return depth;
}

void AdaptiveTrieNode::increaseDepthSize(char c) {
    chars.push_back(c);
}

AdaptiveTrieNode::AdaptiveTrieNode() {
    depth = 0;
    isVirtual = true;
}

size_t AdaptiveTrie::estimateInitMemory(size_t docCount) {
    int k = 126;
    return sizeof(AdaptiveTrie)
           + (k + 1) * (sizeof(AdaptiveTrieNode) + sizeof(char) + sizeof(size_t))// #nodes * class size
           + docCount * sizeof(size_t); // documents distributed to nodes
}

size_t AdaptiveTrie::estimateExtractMemory(size_t valLength) {
    int k = 126;
    return (k * valLength - 1) * (sizeof(AdaptiveTrieNode) + sizeof(char) + sizeof(size_t)); // -1 for node created through initialization
}

/* LENGTH FUNCTIONS */
/*
std::unique_ptr<const DocIndex>
AdaptiveTrie::findLen(const int cmpVal, const AdaptiveIndexQuery::COMPARISON_TYPE &cmpType, DataContext &dataContext) {

    DocIndex result(dataContext.size());
    int maxExtractionDepth = 2;

    AdaptiveTrieNode *currentNode = root.get();

    if (cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS || cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GT || cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GTE) {
        findLenEqualMore(currentNode, cmpVal, cmpType, 0, 0, dataContext, result);
    } else {

    }

    return std::make_unique<const DocIndex>(std::move(result));
}

void AdaptiveTrie::findLenEqualMore(AdaptiveTrieNode *currentNode, int targetLength, const AdaptiveIndexQuery::COMPARISON_TYPE &cmpType, int currentDepth, int currentExtractionDepth, DataContext &dataContext, DocIndex& result) {

    const int maxExtractionDepth = 2;

    if (currentNode->getDepth() >= targetLength) {

        if ((cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS && currentNode->getDepth() == targetLength) || cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GTE) {
            for (size_t i = 0; i < currentNode->exactMatchingDocs.size(); ++i) {
                result.set(currentNode->exactMatchingDocs[i]);
            }
        }

        // Collect all documents with larger content
        if (cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GT || cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GTE) {

            if (currentNode->isVirtual) {
                for (size_t i = 0; i < currentNode->startMatchingDocs.size(); ++i) {
                    result.set(currentNode->startMatchingDocs[i]);
                }
            } else {
                for (auto &child : currentNode->children) {
                    collectDocuments(child.second.get(), result);
                }
            }
        }

        return;
    }

    if (currentNode->isVirtual) {

        if (currentExtractionDepth == 0) {
            dataContext.loadAll(); // different branches, different loads: need to reload if starting a new branch from the top
        }

        if (currentExtractionDepth < maxExtractionDepth && currentNode->getDepth() < 2) {

            if (currentExtractionDepth == (maxExtractionDepth - 1)) {
                std::function<bool(std::string &)> checkFunc;

                switch (cmpType) {
                    case AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS:
                        checkFunc =  [targetLength](std::string &v){ return v.size() == targetLength; };
                        break;
                    case AdaptiveIndexQuery::COMPARISON_TYPE::GT:
                        checkFunc =  [targetLength](std::string &v){ return v.size() > targetLength; };
                        break;
                    case AdaptiveIndexQuery::COMPARISON_TYPE::GTE:
                        checkFunc =  [targetLength](std::string &v){ return v.size() >= targetLength; };
                        break;
                }
                extractAndCheck(currentNode, dataContext, result, checkFunc); // no more extracts happening. Check it!
                return;
            } else {
                extract(currentNode, dataContext);
            }
            currentNode->startMatchingDocs.clear();
            currentNode->startMatchingDocs.shrink_to_fit();

            currentExtractionDepth++;
        } else {
            std::function<bool(std::string &)> checkFunc;

            switch (cmpType) {
                case AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS:
                    checkFunc =  [targetLength](std::string &v){ return v.size() == targetLength; };
                    break;
                case AdaptiveIndexQuery::COMPARISON_TYPE::GT:
                    checkFunc =  [targetLength](std::string &v){ return v.size() > targetLength; };
                    break;
                case AdaptiveIndexQuery::COMPARISON_TYPE::GTE:
                    checkFunc =  [targetLength](std::string &v){ return v.size() >= targetLength; };
                    break;
            }

            check(currentNode, dataContext, result, checkFunc);
            return;
        }
    }

    currentDepth += currentNode->depthSize();

    for(auto &child : currentNode->children) {
        findLenEqualMore(child.second.get(), targetLength, cmpType, currentDepth, currentExtractionDepth, dataContext, result);
    }
}

void AdaptiveTrie::extractAndCheck(AdaptiveTrieNode *node, DataContext &dataContext, DocIndex &result,
                                   std::function<bool(std::string &)> check) {
    if (node->startMatchingDocs.size() == 0) {
        node->isVirtual = false;
        return;
    }

    bool multiExtract = false;

    int depth = node->getDepth();

    do {
        int nextDepth = depth + 1;

        std::unordered_map<char, std::unique_ptr<AdaptiveTrieNode>> &children = node->children;

        // TEMP: Handel root case, Input = Range NOT Ids
        if (depth == 0) {
            dataContext.loadRange(node->startMatchingDocs);

            for (size_t i = 0; i < node->startMatchingDocs.size(); i += 2) {
                for (size_t k = node->startMatchingDocs[i]; k <= node->startMatchingDocs[i + 1]; ++k) {
                    RJValue *val = jsonPathPointer.Get(*dataContext.at(k).getJson());

                    if (!val->IsString()) {
                        std::cout << "not a string but is " << val->GetType() << std::endl;
                    }

                    std::string value = val->GetString();

                    if (check(value)) {
                        result.set(k);
                    }

                    if (children.find(value.at(depth)) == children.end()) {
                        children.insert(std::make_pair(value.at(depth),
                                                       std::make_unique<AdaptiveTrieNode>(value.at(depth), nextDepth)));

                        tmpSizeTracker += (sizeof(char) + sizeof(size_t) + sizeof(AdaptiveTrieNode)); // char + isVirtualBool + depth
                    }

                    if (value.length() == depth) {
                        children.find(value.at(depth))->second->exactMatchingDocs.push_back(k);
                    } else {
                        children.find(value.at(depth))->second->startMatchingDocs.push_back(k);
                    }
                }
            }
        } else {
            dataContext.load(node->startMatchingDocs);
            for (size_t &docIndex : node->startMatchingDocs) {
                RJValue *val = jsonPathPointer.Get(*dataContext.at(docIndex).getJson());

                std::string value = val->GetString();

                if (check(value)) {
                    result.set(docIndex);
                }

                if (children.find(value.at(depth)) == children.end()) {
                    children.insert(std::make_pair(value.at(depth),
                                                   std::make_unique<AdaptiveTrieNode>(value.at(depth), nextDepth)));

                    tmpSizeTracker += (sizeof(char) + sizeof(size_t) + sizeof(AdaptiveTrieNode)); // char + isVirtualBool + depth
                }

                if (value.length() == nextDepth) { // because nextDepth = depth + 1
                    children.find(value.at(depth))->second->exactMatchingDocs.push_back(docIndex);
                } else {
                    children.find(value.at(depth))->second->startMatchingDocs.push_back(docIndex);
                }
            }
        }

        // Compress into current node if there's only one child
        if (children.size() == 1 && node->exactMatchingDocs.size() == 0) {
            node->increaseDepthSize(children.begin()->first);

            // if the one child node has exact matches, transfer these matches to own node
            node->exactMatchingDocs = children.begin()->second->exactMatchingDocs;
            node->startMatchingDocs = children.begin()->second->startMatchingDocs;

            children.clear();
            depth++;
            multiExtract = true;

            // remove the 1 added child, but not the extra char that is saved with increaseDepthSize
            tmpSizeTracker -= (sizeof(char) + sizeof(size_t) + sizeof(AdaptiveTrieNode)); // map char + isVirtualBool + depth
            tmpSizeTracker += sizeof(char); // char in list

        } else {
            multiExtract = false;
        }
    } while (multiExtract);

    node->isVirtual = false;
}

 */

// LEGACY FUNCTION

/*void AdaptiveTrie::check(AdaptiveTrieNode *node, DataContext &dataContext, DocIndex &result,
                                   std::function<bool(std::string &)> check) {
    if (node->startMatchingDocs.size() == 0) {
        node->isVirtual = false;
        return;
    }

    // TEMP: Handel root case, Input = Range NOT Ids
    if (node->getDepth() == 0) {
        dataContext.loadRange(node->startMatchingDocs);

        for (size_t i = 0; i < node->startMatchingDocs.size(); i += 2) {
            for (size_t k = node->startMatchingDocs[i]; k <= node->startMatchingDocs[i + 1]; ++k) {
                RJValue *val = jsonPathPointer.Get(*dataContext.at(k).getJson());

                if (!val->IsString()) {
                    std::cout << "not a string but is " << val->GetType() << std::endl;
                }

                std::string value = val->GetString();

                if (check(value)) {
                    result.set(k);
                }
            }
        }
    } else {
        dataContext.load(node->startMatchingDocs);
        for (size_t &docIndex : node->startMatchingDocs) {
            RJValue *val = jsonPathPointer.Get(*dataContext.at(docIndex).getJson());

            std::string value = val->GetString();

            if (check(value)) {
                result.set(docIndex);
            }
        }
    }
}*/