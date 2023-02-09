#include "AdaptiveQueryCreator.h"
#include <iostream>

std::unique_ptr<AdaptiveIndexQuery> AdaptiveQueryCreator::createQuery(joda::query::EqualProvider *e) {
    auto &lhs = e->getLhs();
    auto &rhs = e->getRhs();
    return createQueryFromEQ(AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS, lhs, rhs);
}

std::unique_ptr<AdaptiveIndexQuery> AdaptiveQueryCreator::createQuery(joda::query::UnequalProvider *e) {
    auto &lhs = e->getLhs();
    auto &rhs = e->getRhs();
    return createQueryFromEQ(AdaptiveIndexQuery::COMPARISON_TYPE::NOT_EQUALS, lhs, rhs);
}

std::unique_ptr<AdaptiveIndexQuery> AdaptiveQueryCreator::createQueryFromEQ(AdaptiveIndexQuery::COMPARISON_TYPE comparisonType, const std::unique_ptr<joda::query::IValueProvider> &lhs, const std::unique_ptr<joda::query::IValueProvider> &rhs) {
    std::unique_ptr<AdaptiveIndexQuery> query;

    std::string pathToProperty;
    RJValue comparisonValue;
    bool isSimpleComparison = false;

    if (lhs->isAny() && rhs->isAtom() && rhs->isConst()) { // lhs = JSON property path; rhs = value that should be equal
        pathToProperty = toJsonPropPath(lhs);
        comparisonValue = getValue(rhs);
        isSimpleComparison = true;
    }

    if (rhs->isAny() && lhs->isAtom() && lhs->isConst()) {
        pathToProperty = toJsonPropPath(rhs);
        comparisonValue = getValue(lhs);
        isSimpleComparison = true;
    }


    if (isSimpleComparison) {
        if (comparisonValue.IsString()) {
            std::string str = comparisonValue.GetString();
            query = std::make_unique<AdaptiveIndexQuery>();
            query->set(pathToProperty, str, comparisonType);
        } else if (comparisonValue.IsNumber() && comparisonValue.IsInt64()) {
            int64_t intVal = comparisonValue.GetInt();
            query = std::make_unique<AdaptiveIndexQuery>();
            query->set(pathToProperty, intVal, comparisonType);
        } else if (comparisonValue.IsTrue()) {
            query = std::make_unique<AdaptiveIndexQuery>();
            query->set(pathToProperty, true, comparisonType);
        } else if (comparisonValue.IsFalse()) {
            query = std::make_unique<AdaptiveIndexQuery>();
            query->set(pathToProperty, false, comparisonType);
        }
    } else {

        std::vector<std::string> lhsAttributes;
        lhs->getAttributes(lhsAttributes);

        std::vector<std::string> rhsAttributes;
        rhs->getAttributes(rhsAttributes);

        if (lhs->getName() == "TYPE" && lhsAttributes.size() > 0) {
            std::string val = getValue(rhs).GetString();

            if (val == "ARRAY") {
                return nullptr;
            }

            query = std::make_unique<AdaptiveIndexQuery>();
            query->set(lhsAttributes[0], getValue(rhs).GetString(), AdaptiveIndexQuery::FUNCTION_TYPE::TYPE, comparisonType);
        } else if (rhs->getName() == "TYPE" && rhsAttributes.size() > 0) {
            std::string val = getValue(lhs).GetString();

            if (val == "ARRAY") {
                return nullptr;
            }

            query = std::make_unique<AdaptiveIndexQuery>();
            query->set(rhsAttributes[0], getValue(lhs).GetString(), AdaptiveIndexQuery::FUNCTION_TYPE::TYPE, comparisonType);
        } else if (lhs->getName() == "LEN" && lhsAttributes.size() > 0) {
            // DISABLED
            //query = std::make_unique<AdaptiveIndexQuery>();
            //query->set(lhsAttributes[0], getValue(rhs).GetInt(), AdaptiveIndexQuery::FUNCTION_TYPE::LEN, AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS);
        } else if (rhs->getName() == "LEN" && rhsAttributes.size() > 0) {
            // DISABLED
            //query = std::make_unique<AdaptiveIndexQuery>();
            //query->set(rhsAttributes[0], getValue(lhs).GetInt(), AdaptiveIndexQuery::FUNCTION_TYPE::LEN, AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS);
        }
        else {
            return nullptr;
        }
    }

    return query;
}

std::unique_ptr<AdaptiveIndexQuery> AdaptiveQueryCreator::createQuery(joda::query::LessProvider *e) {
    auto &lhs = e->getLhs();
    auto &rhs = e->getRhs();
    return createQueryFromComp(AdaptiveIndexQuery::COMPARISON_TYPE::LT, lhs, rhs);
}

std::unique_ptr<AdaptiveIndexQuery> AdaptiveQueryCreator::createQuery(joda::query::LessEqualProvider *e) {
    auto &lhs = e->getLhs();
    auto &rhs = e->getRhs();
    return createQueryFromComp(AdaptiveIndexQuery::COMPARISON_TYPE::LTE, lhs, rhs);
}

std::unique_ptr<AdaptiveIndexQuery> AdaptiveQueryCreator::createQuery(joda::query::GreaterEqualProvider *e) {
    auto &lhs = e->getLhs();
    auto &rhs = e->getRhs();
    return createQueryFromComp(AdaptiveIndexQuery::COMPARISON_TYPE::GTE, lhs, rhs);
}

std::unique_ptr<AdaptiveIndexQuery> AdaptiveQueryCreator::createQuery(joda::query::GreaterProvider *e) {
    auto &lhs = e->getLhs();
    auto &rhs = e->getRhs();
    return createQueryFromComp(AdaptiveIndexQuery::COMPARISON_TYPE::GT, lhs, rhs);
}


std::unique_ptr<AdaptiveIndexQuery> AdaptiveQueryCreator::createQueryFromComp(AdaptiveIndexQuery::COMPARISON_TYPE comparisonType, const std::unique_ptr<joda::query::IValueProvider> &lhs, const std::unique_ptr<joda::query::IValueProvider> &rhs) {
    std::unique_ptr<AdaptiveIndexQuery> query;


    std::string pathToProperty;
    RJValue comparisonValue;

    if (lhs->isAny() && rhs->isAtom() && rhs->isConst()) { // lhs = JSON property path; rhs = value that should be equal
        pathToProperty = toJsonPropPath(lhs);
        comparisonValue = getValue(rhs);
    }

    if (rhs->isAny() && lhs->isAtom() && lhs->isConst()) {
        pathToProperty = toJsonPropPath(rhs);
        comparisonValue = getValue(lhs);
    }

    if (comparisonValue.IsNumber() || comparisonValue.IsString()) {

        if (comparisonValue.IsNumber() && comparisonValue.IsInt64()) {
            int64_t intVal = comparisonValue.GetInt64();
            query = std::make_unique<AdaptiveIndexQuery>();
            query->set(pathToProperty, intVal, comparisonType);
        } else if (comparisonValue.IsString()) {
            std::string strVal = comparisonValue.GetString();
            query = std::make_unique<AdaptiveIndexQuery>();
            query->set(pathToProperty, strVal, comparisonType);
        }
    } else {
        return nullptr;
        // DISABLE LEN
        /*std::vector<std::string> lhsAttributes;
        lhs->getAttributes(lhsAttributes);

        std::vector<std::string> rhsAttributes;
        rhs->getAttributes(rhsAttributes);

        if (lhs->getName() == "LEN" && lhsAttributes.size() > 0) {
            query = std::make_unique<AdaptiveIndexQuery>();
            query->set(lhsAttributes[0], getValue(rhs).GetInt(), AdaptiveIndexQuery::FUNCTION_TYPE::LEN, getComparisonType(e));
        } else if (rhs->getName() == "LEN" && rhsAttributes.size() > 0) {
            query = std::make_unique<AdaptiveIndexQuery>();
            query->set(rhsAttributes[0], getValue(lhs).GetInt(), AdaptiveIndexQuery::FUNCTION_TYPE::LEN, getComparisonType(e));
        } else {
            return nullptr;
        }*/
    }

    return query;
}

std::unique_ptr<AdaptiveIndexQuery> AdaptiveQueryCreator::createQuery(joda::query::IValueProvider *e) {
    std::unique_ptr<AdaptiveIndexQuery> query = std::make_unique<AdaptiveIndexQuery>();

    std::vector<std::string> attributes;
    e->getAttributes(attributes);
    auto name = e->getName();

    if (name == "ISNULL") {
        query->set(attributes[0], AdaptiveIndexQuery::FUNCTION_TYPE::ISNULL);
    } else if (name == "ISBOOL") {
        query->set(attributes[0], AdaptiveIndexQuery::FUNCTION_TYPE ::ISBOOL);
    } else if (name == "ISNUMBER") {
        query->set(attributes[0], AdaptiveIndexQuery::FUNCTION_TYPE ::ISNUMBER);
    } else if (name == "ISSTRING") {
        query->set(attributes[0], AdaptiveIndexQuery::FUNCTION_TYPE ::ISSTRING);
    } else if (name == "ISOBJECT") {
        query->set(attributes[0], AdaptiveIndexQuery::FUNCTION_TYPE ::ISOBJECT);
    } else if (name == "EXISTS") {
        query->set(attributes[0], AdaptiveIndexQuery::FUNCTION_TYPE ::EXISTS);
    } else if (name == "STARTSWITH") {
        std::string swString = e->toString();
        std::vector<std::string> attributes;
        e->getAttributes(attributes);

        std::string startsWith = swString.substr(swString.find_first_of("\"") + 1, swString.size() - (swString.find_first_of("\"") + 3));
        query->set(attributes[0], startsWith, AdaptiveIndexQuery::COMPARISON_TYPE::STARTS_WITH);
    } else if (name == "LESS"){
        return createQuery(static_cast<joda::query::LessProvider*>(e));
    }else if (name == "LESSEQ"){
        return createQuery(static_cast<joda::query::LessEqualProvider*>(e));
    }else if (name == "GREATEREQ"){
        return createQuery(static_cast<joda::query::GreaterEqualProvider*>(e));
    }else if (name == "GREATER"){
        return createQuery(static_cast<joda::query::GreaterProvider*>(e));
    }else if (name == "EQUAL"){
        return createQuery(static_cast<joda::query::EqualProvider*>(e));
    }else if (name == "UNEQUAL"){
        return createQuery(static_cast<joda::query::UnequalProvider*>(e));
    }else {
        return nullptr;
    }


    return query;
}

std::string AdaptiveQueryCreator::toJsonPropPath(const std::unique_ptr<joda::query::IValueProvider> &valueProvider) {
    std::string path = valueProvider->toString();
    path = valueProvider->toString().substr(1, path.size() - 2);
    return path;
}

RJValue AdaptiveQueryCreator::getValue(const std::unique_ptr<joda::query::IValueProvider> &valueProvider) {
    RJMemoryPoolAlloc alloc;
    return valueProvider->getAtomValue(RapidJsonDocument(), alloc);
}
