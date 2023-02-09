#include <joda/indexing/AdaptiveIndexQuery.h>

void AdaptiveIndexQuery::set(std::string &pPath, std::string value, const AdaptiveIndexQuery::COMPARISON_TYPE cType) {
    propertyPath = pPath;
    strValue = value;
    cmpType = cType;
    contentType = CONTENT_TYPE::STRING;
}

void AdaptiveIndexQuery::set(std::string &pPath, std::string value, const AdaptiveIndexQuery::FUNCTION_TYPE fType, const AdaptiveIndexQuery::COMPARISON_TYPE cType) {
    propertyPath = pPath;
    strValue = value;
    functionType = fType;
    contentType = CONTENT_TYPE::STRING;
    cmpType = cType;
}

void AdaptiveIndexQuery::set(std::string &pPath, int64_t value, const AdaptiveIndexQuery::FUNCTION_TYPE fType, const AdaptiveIndexQuery::COMPARISON_TYPE cType) {
    propertyPath = pPath;
    intVal = value;
    functionType = fType;
    cmpType = cType;
    contentType = CONTENT_TYPE::INT;
}

void AdaptiveIndexQuery::set(std::string &pPath, bool value, const AdaptiveIndexQuery::COMPARISON_TYPE cType) {
    propertyPath = pPath;
    boolVal = value;
    cmpType = cType;
    contentType = CONTENT_TYPE::BOOL;
}


void AdaptiveIndexQuery::set(std::string &pPath, int64_t value, const COMPARISON_TYPE cType) {
    propertyPath = pPath;
    intVal = value;
    cmpType = cType;
    contentType = CONTENT_TYPE::INT;
}

void AdaptiveIndexQuery::set(std::string &pPath, const FUNCTION_TYPE fType) {
    propertyPath = pPath;
    functionType = fType;
    contentType = CONTENT_TYPE::NONE;
}

bool AdaptiveIndexQuery::equals(const AdaptiveIndexQuery &query) const {
    return contentType == query.contentType
               && functionType == query.functionType
               && cmpType == query.cmpType
               && conjunctionType == query.conjunctionType
               && intVal == query.intVal
               && propertyPath == query.propertyPath
               && boolVal == query.boolVal
               && strValue == query.strValue;
}
