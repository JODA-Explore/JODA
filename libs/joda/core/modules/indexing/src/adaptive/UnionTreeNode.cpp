#include "UnionTreeNode.h"

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <joda/misc/MemoryUtility.h>

UnionTreeNode::UnionTreeNode(std::string jp) : ManagedIndexData(jp, ManagedDataType::UnionTreeNodeData) {

}

std::unique_ptr<const DocIndex> UnionTreeNode::getDocumentsFirst(
        const AdaptiveIndexQuery &query,
        std::shared_ptr<UnionTreeNode> parentNode,
        std::shared_ptr<UnionTreeNode> parentDocRangeNode,
        DataContext& dataContext) {

    bool knownNodeIsDirectParent = parentNode->jsonPath == parentDocRangeNode->jsonPath;

    if (!dataContext.getMemoryManager()->ensureMemory(getEstimatedFirstMemory(parentDocRangeNode->getDocCount(), knownNodeIsDirectParent, query.contentType), this)) {
        return nullptr;
    }

    const std::vector<size_t> &parentObjectDocs = parentDocRangeNode->getObjectDocs();
    dataContext.loadRange(parentObjectDocs);

    DocIndex result(dataContext.size());

    UnionTreeNodeDocumentHelper parentNodeDocumentHelper;
    UnionTreeNodeDocumentHelper nodeDocumentHelper;

    auto parentNodePointer = RJPointer(parentNode->jsonPath.c_str());
    auto nodePointer = RJPointer(query.propertyPath.c_str());

    std::string memberName = query.propertyPath.substr(query.propertyPath.rfind("/") + 1);

    if (query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::STRING) {
        createAdaptiveTrieIndex(dataContext, false);
    } else if (query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::INT) {
        nodeDocumentHelper.numberValues = std::make_unique<std::vector<int64_t>>();
    }

    for (uint j = 0; j < parentObjectDocs.size(); j += 2) {

        for (uint k = parentObjectDocs[j]; k <= parentObjectDocs[j + 1]; ++k) {

            const RJValue *val;
            if (knownNodeIsDirectParent) {
                val = dataContext.at(k).Get(nodePointer);
            } else {
                val = dataContext.at(k).Get(parentNodePointer);

                if (handleValue(&parentNodeDocumentHelper, val, k)) {
                    auto m = val->FindMember(memberName);
                    if (m != val->MemberEnd()) {
                        val = &m->value;
                    } else {
                        val = nullptr;
                    }
                }
            }

            if (val != nullptr) {
                nodeDocumentHelper.totalDocs++;

                switch (val->GetType()) {
                    case rapidjson::Type::kStringType:
                        if (query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::STRING) {

                            std::string value = val->GetString();

                            adaptiveTrie->initializeByStep(value, k);

                            if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS) {
                                if (value == query.strValue) {
                                    result.set(k);
                                }
                            } else if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::NOT_EQUALS) {
                                if (value != query.strValue) {
                                    result.set(k);
                                }
                            } else if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::STARTS_WITH) {

                                if (value.size() >= query.strValue.size() &&
                                    value.substr(0, query.strValue.size()) == query.strValue) {
                                    result.set(k);
                                }

                                // possibly create the 1st level trie index here?
                            } else { // lexicographic comparison
                                switch (query.cmpType) {
                                    case AdaptiveIndexQuery::COMPARISON_TYPE::LT:
                                        if (value < query.strValue) result.set(k);
                                        break;
                                    case AdaptiveIndexQuery::COMPARISON_TYPE::LTE:
                                        if (value <= query.strValue) result.set(k);
                                        break;
                                    case AdaptiveIndexQuery::COMPARISON_TYPE::GT:
                                        if (value > query.strValue) result.set(k);
                                        break;
                                    case AdaptiveIndexQuery::COMPARISON_TYPE::GTE:
                                        if (value >= query.strValue) result.set(k);
                                        break;
                                    default:
                                        break;
                                }
                            }
                        } else if (query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::LEN) {
                            switch (query.cmpType) {
                                case AdaptiveIndexQuery::COMPARISON_TYPE ::EQUALS:
                                    if (val->GetStringLength() == query.intVal) result.set(k);
                                    break;
                                case AdaptiveIndexQuery::COMPARISON_TYPE::LT:
                                    if (val->GetStringLength() < query.intVal) result.set(k);
                                    break;
                                case AdaptiveIndexQuery::COMPARISON_TYPE::LTE:
                                    if (val->GetStringLength() <= query.intVal) result.set(k);
                                    break;
                                case AdaptiveIndexQuery::COMPARISON_TYPE::GT:
                                    if (val->GetStringLength() > query.intVal) result.set(k);
                                    break;
                                case AdaptiveIndexQuery::COMPARISON_TYPE::GTE:
                                    if (val->GetStringLength() >= query.intVal) result.set(k);
                                    break;
                                default:
                                    break;
                            }
                        }

                        if (!nodeDocumentHelper.isWithinValidStringRange) {
                            nodeDocumentHelper.checkString(k);
                        }
                        break;
                    case rapidjson::Type::kNumberType: {
                            if (val->IsInt64()) {
                                int64_t intVal = val->GetInt64();

                                if (nodeDocumentHelper.intMin > intVal) {
                                    nodeDocumentHelper.intMin = intVal;
                                } else if (nodeDocumentHelper.intMax < intVal) {
                                    nodeDocumentHelper.intMax = intVal;
                                }

                                if (query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::INT) {

                                    nodeDocumentHelper.numberValues->push_back(intVal);

                                    if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS || query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::NOT_EQUALS) {
                                        // NOT_EQUALS will be flipped later
                                        if (intVal == query.intVal) {
                                            result.set(k);
                                        }
                                    } else if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LT) {
                                        if (intVal < query.intVal) {
                                            result.set(k);
                                        }
                                    } else if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LTE) {
                                        if (intVal <= query.intVal) {
                                            result.set(k);
                                        }
                                    } else if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GT) {
                                        if (intVal > query.intVal) {
                                            result.set(k);
                                        }
                                    } else if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GTE) {
                                        if (intVal >= query.intVal) {
                                            result.set(k);
                                        }
                                    }
                                }
                            } else {
                                nodeDocumentHelper.unsupportedDouble = true;

                                // set result correct for this part
                                if (query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::INT && val->IsDouble()) {

                                    double dVal = val->GetDouble();

                                    if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS || query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::NOT_EQUALS) {
                                        // NOT_EQUALS will be flipped later
                                        if (dVal == query.intVal) {
                                            result.set(k);
                                        }
                                    } else if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LT) {
                                        if (dVal < query.intVal) {
                                            result.set(k);
                                        }
                                    } else if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LTE) {
                                        if (dVal <= query.intVal) {
                                            result.set(k);
                                        }
                                    } else if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GT) {
                                        if (dVal > query.intVal) {
                                            result.set(k);
                                        }
                                    } else if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GTE) {
                                        if (dVal >= query.intVal) {
                                            result.set(k);
                                        }
                                    }
                                }
                            }

                            if (!nodeDocumentHelper.isWithinValidNumberRange) {
                                nodeDocumentHelper.checkNumber(k);
                            }
                        } break;
                    case rapidjson::Type::kObjectType:
                        if (!nodeDocumentHelper.isWithinValidObjectRange) {
                            nodeDocumentHelper.checkObject(k);
                        }
                        break;
                    case rapidjson::Type::kTrueType:
                        if (query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::BOOL && query.boolVal) {
                            result.set(k);
                        }

                        if (!nodeDocumentHelper.isWithinValidTrueRange) {
                            nodeDocumentHelper.checkTrue(k);
                        }
                        break;
                    case rapidjson::Type::kFalseType:
                        if (query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::BOOL && !query.boolVal) {
                            result.set(k);
                        }

                        if (!nodeDocumentHelper.isWithinValidFalseRange) {
                            nodeDocumentHelper.checkFalse(k);
                        }
                        break;
                    case rapidjson::Type::kNullType:
                        if (query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::ISNULL) {
                            result.set(k);
                        }

                        if (!nodeDocumentHelper.isWithinValidNullRange) {
                            nodeDocumentHelper.checkNull(k);
                        }
                        break;
                    default:
                        // else: array => ignore
                        break;
                }
                // else: array / null => ignore
            } else {
                nodeDocumentHelper.resetRange(k - 1);
            }
        }
        parentNodeDocumentHelper.resetRange(parentObjectDocs[j + 1]);
        nodeDocumentHelper.resetRange(parentObjectDocs[j + 1]);
    }

    if (!knownNodeIsDirectParent) {
        parentNode->stringDocuments = std::move(parentNodeDocumentHelper.stringDocuments);

        parentNode->objectDocuments = std::move(parentNodeDocumentHelper.objectDocuments);
        parentNode->trueDocuments = std::move(parentNodeDocumentHelper.trueDocuments);
        parentNode->falseDocuments = std::move(parentNodeDocumentHelper.falseDocuments);
        parentNode->nullDocuments = std::move(parentNodeDocumentHelper.nullDocuments);

        parentNode->relevantDocumentsCalculated = true;

        parentNode->numberDocuments = std::move(parentNodeDocumentHelper.numberDocuments);

        if (!parentNodeDocumentHelper.unsupportedDouble) {
            parentNode->numberIndex = std::make_unique<NumberIndex>(*parentNode->numberDocuments, parentNodeDocumentHelper.intMin, parentNodeDocumentHelper.intMax, jsonPath);
            parentNode->numberIndex->used(dataContext);
        }


        parentNode->nodeUsed(dataContext);
        parentDocRangeNode->nodeUsed(dataContext);
    } else {
        parentDocRangeNode->nodeUsed(dataContext);
    }

    relevantDocumentsCalculated = true;
    stringDocuments = std::move(nodeDocumentHelper.stringDocuments);
    objectDocuments = std::move(nodeDocumentHelper.objectDocuments);
    trueDocuments = std::move(nodeDocumentHelper.trueDocuments);
    falseDocuments = std::move(nodeDocumentHelper.falseDocuments);
    nullDocuments = std::move(nodeDocumentHelper.nullDocuments);

    numberDocuments = std::move(nodeDocumentHelper.numberDocuments);

    if (!nodeDocumentHelper.unsupportedDouble) {
        numberIndex = std::make_unique<NumberIndex>(*numberDocuments, nodeDocumentHelper.intMin, nodeDocumentHelper.intMax, jsonPath);

        if (nodeDocumentHelper.numberValues) {
            numberIndex->CreateHistogramIndexWithValues(std::move(nodeDocumentHelper.numberValues), dataContext);
        }
        numberIndex->used(dataContext);
    }
    totalDocs = nodeDocumentHelper.totalDocs;

    switch (query.functionType) {
        case AdaptiveIndexQuery::FUNCTION_TYPE ::EXISTS:
            setResult(result, stringDocuments.get());
            setResult(result, objectDocuments.get());
            setResult(result, trueDocuments.get());
            setResult(result, falseDocuments.get());
            setResult(result, numberDocuments.get());
            setResult(result, nullDocuments.get());
        case AdaptiveIndexQuery::FUNCTION_TYPE ::TYPE:
            if (query.strValue == "STRING") {
                setResult(result, stringDocuments.get());
            } else if (query.strValue == "NUMBER") {
                setResult(result, numberDocuments.get());
            } else if (query.strValue == "BOOL") {
                setResult(result, trueDocuments.get());
                setResult(result, falseDocuments.get());
            } else if (query.strValue == "NULL") {
                setResult(result, nullDocuments.get());
            } else if (query.strValue == "OBJECT") {
                setResult(result, objectDocuments.get());
            } else if (query.strValue == "ARRAY") {
                // NOT supported
            }

            break;
        case AdaptiveIndexQuery::FUNCTION_TYPE::ISBOOL:
            setResult(result, trueDocuments.get());
            setResult(result, falseDocuments.get());
            break;
        case AdaptiveIndexQuery::FUNCTION_TYPE::ISNUMBER:
            setResult(result, numberDocuments.get());
            break;
        case AdaptiveIndexQuery::FUNCTION_TYPE::ISSTRING:
            setResult(result, stringDocuments.get());
            break;
        case AdaptiveIndexQuery::FUNCTION_TYPE::ISOBJECT:
            setResult(result, objectDocuments.get());
            break;
        default:
            break;
    }

    if (query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::NONE && query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::BOOL && query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::NOT_EQUALS) {
        result.flip();
    } else if (query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::NONE && query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::INT && query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::NOT_EQUALS) {
        result.flip();
    } else if (query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::TYPE && query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::NOT_EQUALS) {
        result.flip();
    }

    if (query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::STRING) {
        adaptiveTrie->finishStepInitialization(dataContext);
    }

    return std::make_unique<const DocIndex>(std::move(result));
}

std::unique_ptr<const DocIndex> UnionTreeNode::getDocumentsSecond(
        const AdaptiveIndexQuery &query,
        DataContext& dataContext) {

    if (query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::TYPE) {
        DocIndex result(dataContext.size());
        if (query.strValue == "STRING") {
            setResult(result, stringDocuments.get());
        } else if (query.strValue == "NUMBER") {
            setResult(result, numberDocuments.get());
        } else if (query.strValue == "BOOL") {
            setResult(result, trueDocuments.get());
            setResult(result, falseDocuments.get());
        } else if (query.strValue == "NULL") {
            setResult(result, nullDocuments.get());
        } else if (query.strValue == "OBJECT") {
            setResult(result, objectDocuments.get());
        }

        if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::NOT_EQUALS) {
            result.flip();
        }

        return std::make_unique<const DocIndex>(std::move(result));
    } else if (query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::ISBOOL) {
        DocIndex result(dataContext.size());
        setResult(result, trueDocuments.get());
        setResult(result, falseDocuments.get());
        return std::make_unique<const DocIndex>(std::move(result));
    } else if (query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::ISNUMBER) {
        DocIndex result(dataContext.size());
        setResult(result, numberDocuments.get());
        return std::make_unique<const DocIndex>(std::move(result));
    } else if (query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::ISSTRING) {
        DocIndex result(dataContext.size());
        setResult(result, stringDocuments.get());
        return std::make_unique<const DocIndex>(std::move(result));
    } else if (query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::ISOBJECT) {
        DocIndex result(dataContext.size());
        setResult(result, objectDocuments.get());
        return std::make_unique<const DocIndex>(std::move(result));
    } else if (query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::STRING || query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::LEN) {

        if (adaptiveTrie) {
            if (!dataContext.getMemoryManager()->ensureMemory(AdaptiveTrie::estimateExtractMemory(query.strValue.length()), this)) {
                return nullptr;
            }
        } else {
            if (!dataContext.getMemoryManager()->ensureMemory(
                    AdaptiveTrie::estimateInitMemory(getDocCount()) + AdaptiveTrie::estimateExtractMemory(query.strValue.length()), this)) {
                return nullptr;
            }

            createAdaptiveTrieIndex(dataContext, true);
        }

        if (query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::LEN) {
            return nullptr; // not activated
            //return adaptiveTrie->findLen(query.intVal, query.cmpType, dataContext);
        } else if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS) {
            return adaptiveTrie->findEq(query.strValue, dataContext);
        } else if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::NOT_EQUALS) {
            return adaptiveTrie->findNotEq(query.strValue, dataContext, *stringDocuments.get());
        } else if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::STARTS_WITH) {
            return adaptiveTrie->findStartsWith(query.strValue, dataContext);
        } else {
            // lexicographic comparison

            if (query.strValue.size() == 0) {
                if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::LT) {
                    DocIndex result(dataContext.size());
                    return std::make_unique<const DocIndex>(std::move(result));
                } else if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::GTE) {
                    DocIndex result(dataContext.size());
                    setResult(result, stringDocuments.get());
                    return std::make_unique<const DocIndex>(std::move(result));;
                }
            }

            return adaptiveTrie->findLex(query.strValue, query.cmpType, dataContext);
        }

    } else if (query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::ISNULL) {
        DocIndex result(dataContext.size());
        setResult(result, nullDocuments.get());
        return std::make_unique<const DocIndex>(std::move(result));
    } else if (query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::EXISTS) {
        DocIndex result(dataContext.size());

        setResult(result, stringDocuments.get());
        setResult(result, objectDocuments.get());
        setResult(result, trueDocuments.get());
        setResult(result, falseDocuments.get());
        setResult(result, numberDocuments.get());
        setResult(result, nullDocuments.get());

        return std::make_unique<const DocIndex>(std::move(result));
    } else if (query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::BOOL) {
        DocIndex result(dataContext.size());

        if (query.boolVal) {
            setResult(result, trueDocuments.get());
        } else {
            setResult(result, falseDocuments.get());
        }

        if (query.cmpType == AdaptiveIndexQuery::COMPARISON_TYPE::NOT_EQUALS) {
            result.flip();
        }

        return std::make_unique<const DocIndex>(std::move(result));

    } else if (query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::INT) {

        if (!numberIndex) {
            return nullptr;
        }

        int neededMemory = numberIndex->isInitialized() ?
                AdaptiveHistogram::estimateSplitMemory() :
                AdaptiveHistogram::estimateInitMemory(getDocCount()) + AdaptiveHistogram::estimateSplitMemory();

        if (!dataContext.getMemoryManager()->ensureMemory(neededMemory, this)) {
            return nullptr;
        }

        return std::move(numberIndex->find(query.intVal, query.cmpType, dataContext));
    }

    return nullptr;
}

void UnionTreeNode::setResult(DocIndex &result, std::vector<size_t> *range) {
    for (size_t j = 0; j < range->size(); j += 2) {
        for (size_t k = range->at(j); k <= range->at(j+1); ++k) {
            result.set(k);
        }
    }
}

bool UnionTreeNode::handleValue(UnionTreeNodeDocumentHelper *nodeDocumentHelper, const RJValue *val, int k) {
    bool isObject = false;
    nodeDocumentHelper->totalDocs++;

    if (val != nullptr) {

        switch (val->GetType()) {
            case rapidjson::Type::kStringType:
                if (!nodeDocumentHelper->isWithinValidStringRange) {
                    nodeDocumentHelper->checkString(k);
                }
                break;
            case rapidjson::Type::kNumberType: {
                    if (val->IsInt64()) {
                        int64_t intVal = val->GetInt64();

                        if (nodeDocumentHelper->intMin > intVal) {
                            nodeDocumentHelper->intMin = intVal;
                        } else if (nodeDocumentHelper->intMax < intVal) {
                            nodeDocumentHelper->intMax = intVal;
                        }

                        if (!nodeDocumentHelper->isWithinValidNumberRange) {
                            nodeDocumentHelper->checkNumber(k);
                        }
                    } else if (val->IsDouble()) {
                        nodeDocumentHelper->unsupportedDouble = true;
                    }

                    if (!nodeDocumentHelper->isWithinValidNumberRange) {
                        nodeDocumentHelper->checkNumber(k);
                    }
                } break;
            case rapidjson::Type::kObjectType:
                isObject = true;
                if (!nodeDocumentHelper->isWithinValidObjectRange) {
                    nodeDocumentHelper->checkObject(k);
                }
                break;
            case rapidjson::Type::kTrueType:
                if (!nodeDocumentHelper->isWithinValidTrueRange) {
                    nodeDocumentHelper->checkTrue(k);
                }
                break;
            case rapidjson::Type::kFalseType:
                if (!nodeDocumentHelper->isWithinValidFalseRange) {
                    nodeDocumentHelper->checkFalse(k);
                }
                break;
            case rapidjson::Type::kNullType:
                if (!nodeDocumentHelper->isWithinValidNullRange) {
                    nodeDocumentHelper->checkNull(k);
                }
                break;
            default:
                // else: array => ignore
                break;
        }
    } else {
        nodeDocumentHelper->resetRange(k - 1);
    }

    return isObject;
}

void UnionTreeNode::setRootDocuments(size_t totalDocs, const DataContext &dataContext) {
    std::vector<size_t> allDocuments;
    allDocuments.push_back(0);
    allDocuments.push_back(totalDocs - 1);

    stringDocuments = std::make_unique<std::vector<size_t>>(allDocuments);
    numberDocuments = std::make_unique<std::vector<size_t>>(allDocuments);
    objectDocuments = std::make_unique<std::vector<size_t>>(allDocuments);
    trueDocuments = std::make_unique<std::vector<size_t>>(allDocuments);
    falseDocuments = std::make_unique<std::vector<size_t>>(allDocuments);
    nullDocuments = std::make_unique<std::vector<size_t>>(allDocuments);

    relevantDocumentsCalculated = true;
    this->totalDocs = totalDocs;
}

bool UnionTreeNode::isRelevantDocumentsCalculated() const {
    return relevantDocumentsCalculated;
}


const std::vector<size_t> &UnionTreeNode::getObjectDocs() const {
    return *objectDocuments;
}

size_t UnionTreeNode::getDocCount() const {
    return totalDocs;
}

size_t UnionTreeNode::estimateWork(const AdaptiveIndexQuery &query) {
    if (query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::EXISTS ||
            query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::ISNULL ||
            query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::ISBOOL ||
            query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::ISNULL ||
            query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::ISNUMBER ||
            query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::ISNULL ||
            query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::ISOBJECT ||
            query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::ISSTRING ||
            query.functionType == AdaptiveIndexQuery::FUNCTION_TYPE::TYPE
    ) {
        return 1;
    } else if (query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::STRING) {
        if (adaptiveTrie) {
            return adaptiveTrie->estimateWork(query);
        }

        // These are range elements not document indexes, so this is a very very rough upper estimate for the effort.
        return totalDocs - numberDocuments->size() - falseDocuments->size() - nullDocuments->size() - objectDocuments->size() - trueDocuments->size();
    } else if (query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::BOOL) {
        return 1;
    } else if (query.contentType == AdaptiveIndexQuery::CONTENT_TYPE::INT) {
        if (!numberIndex) {
            return ULONG_MAX;
        }

        if (numberIndex->isInitialized()) {
            return numberIndex->estimateWork(query);
        }

        return totalDocs - stringDocuments->size() - falseDocuments->size() - nullDocuments->size() - objectDocuments->size() - trueDocuments->size();
    } else {
        return totalDocs;
    }
}

void UnionTreeNode::nodeUsed(const DataContext &dataContext) {
    size_t size = sizeof(UnionTreeNode);

    if (relevantDocumentsCalculated) {
        size += sizeof(size_t) * (stringDocuments->size() + numberDocuments->size() + objectDocuments->size()
        + trueDocuments->size() + falseDocuments->size() + nullDocuments->size())

        + 6 * sizeof(std::vector<size_t>);
    }

    setSize(size);
    updateLastUsed(dataContext);
}

void UnionTreeNode::removeElement(ManagedDataType dataType) {
    switch (dataType) {
        case ManagedDataType::AdaptiveTrieData:
            std::cout << "sys usage before: " << MemoryUtility::sysRamUsage().getHumanReadable() << std::endl;
            adaptiveTrie.reset();
            std::cout << "sys usage after: " << MemoryUtility::sysRamUsage().getHumanReadable() << std::endl;
            break;
        case ManagedDataType::NumberIndexData:
            std::cout << "sys usage before: " << MemoryUtility::sysRamUsage().getHumanReadable() << std::endl;
            numberIndex.reset();
            std::cout << "sys usage after: " << MemoryUtility::sysRamUsage().getHumanReadable() << std::endl;
            break;
    }
}

void UnionTreeNode::createAdaptiveTrieIndex(DataContext &dataContext, bool fullInit) {

    if (!adaptiveTrie) {
        adaptiveTrie = std::make_unique<AdaptiveTrie>(jsonPath);
        adaptiveTrie->initialize(dataContext, *stringDocuments, fullInit);
    }
}

size_t UnionTreeNode::getEstimatedFirstMemory(size_t docCount, bool knownNodeIsDirectParent,
                                 AdaptiveIndexQuery::CONTENT_TYPE contentType) {
    size_t neededSize = 0;

    if (knownNodeIsDirectParent) {
        neededSize = docCount * 2 * sizeof(size_t); // worst case
        neededSize += sizeof(NumberIndex);
    } else {
        neededSize = docCount * 2 * 2 * sizeof(size_t); // same but two nodes are filled with content
        neededSize += 2 * sizeof(NumberIndex);
    }

    if (contentType == AdaptiveIndexQuery::CONTENT_TYPE::STRING) {
        neededSize += AdaptiveTrie::estimateInitMemory(docCount);
    } else if (contentType == AdaptiveIndexQuery::CONTENT_TYPE::INT) {
        neededSize += AdaptiveHistogram::estimateInitMemory(docCount);
    }

    return neededSize;
}

/**
 * Example function to compress a vector of number.
 */
void UnionTreeNode::compress() {

    /*auto start = std::chrono::high_resolution_clock::now();

    std::cout << "Compressing " << jsonPath << std::endl;

    SIMDCompressionLib::IntegerCODEC &codec = *SIMDCompressionLib::CODECFactory::getFromName("s4-bp128-dm");

    // String Docs compression
    std::vector<uint32_t> compressed_output(stringDocuments.size() + 1024);
    std::size_t compressedsize = compressed_output.size();

    if (stringDocuments->size() > 0) {
        codec.encodeArray(stringDocuments->data(), stringDocuments.size(), compressed_output.data(), compressedsize);
        compressed_output.resize(compressedsize);
        compressed_output.shrink_to_fit();

        // display compression rate:
        std::cout << std::setprecision(3);
        std::cout << "You are using "
                  << 32.0 * static_cast<double>(compressed_output.size()) /
                     static_cast<double>(stringDocuments.size())
                  << " bits per integer. Original #" << stringDocuments.size() << std::endl;

        std::cout << "Array length "
                  << (static_cast<double>(compressed_output.size()) /
                      static_cast<double>(stringDocuments.size())) * 100
                  << " %" << std::endl;

        stringDocuments = compressed_output;
    }

    // Number Docs compression
    if (numberDocuments->size() > 0) {
        compressed_output = std::vector<uint32_t>(numberDocuments->size() + 1024);
        compressedsize = compressed_output.size();
        codec.encodeArray(numberDocuments->data(), numberDocuments->size(), compressed_output.data(), compressedsize);
        compressed_output.resize(compressedsize);
        compressed_output.shrink_to_fit();

        // display compression rate:
        std::cout << std::setprecision(3);
        std::cout << "You are using "
                  << 32.0 * static_cast<double>(compressed_output.size()) /
                     static_cast<double>(numberDocuments->size())
                  << " bits per integer. Original #" << numberDocuments->size() << std::endl;

        numberDocuments = compressed_output;
    }

    // Object Docs compression
    if (objectDocuments->size()) {
        compressed_output = std::vector<uint32_t>(objectDocuments->size() + 1024);
        compressedsize = compressed_output.size();
        codec.encodeArray(objectDocuments->data(), objectDocuments->size(), compressed_output.data(), compressedsize);
        compressed_output.resize(compressedsize);
        compressed_output.shrink_to_fit();

        // display compression rate:
        std::cout << std::setprecision(3);
        std::cout << "You are using "
                  << 32.0 * static_cast<double>(compressed_output.size()) /
                     static_cast<double>(objectDocuments->size())
                  << " bits per integer. Original #" << objectDocuments->size() << std::endl;

        objectDocuments = compressed_output;
    }
    compressed = true;

    auto finish = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);
    std::cout << "Compression finished in " << microseconds.count() << "µs\n";*/
}