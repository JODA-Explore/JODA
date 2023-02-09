#pragma once

#include "joda/document/RapidJsonDocument.h"

struct AdaptiveIndexQuery {
    enum class COMPARISON_TYPE { EQUALS, NOT_EQUALS, STARTS_WITH, LT, LTE, GT, GTE };
    enum class CONTENT_TYPE { STRING, INT, BOOL, NONE };
    enum class FUNCTION_TYPE { ISNULL, ISBOOL, ISNUMBER, ISSTRING, ISOBJECT, EXISTS, TYPE, LEN, NONE };
    enum class CONJUNCTION_TYPE { NONE, AND, OR };

    std::string strValue;
    bool boolVal;
    int64_t intVal;
    std::string propertyPath;
    COMPARISON_TYPE cmpType;
    CONTENT_TYPE contentType;
    FUNCTION_TYPE functionType = FUNCTION_TYPE::NONE;
    CONJUNCTION_TYPE conjunctionType = CONJUNCTION_TYPE::NONE;

    void set(std::string &propertyPath, std::string value, const COMPARISON_TYPE cmpType);
    void set(std::string &propertyPath, std::string value, const FUNCTION_TYPE fType, const AdaptiveIndexQuery::COMPARISON_TYPE cType);
    void set(std::string &pPath, int64_t value, const AdaptiveIndexQuery::FUNCTION_TYPE fType, const AdaptiveIndexQuery::COMPARISON_TYPE cType);
    void set(std::string &propertyPath, bool value, const COMPARISON_TYPE cmpType);
    void set(std::string &propertyPath, int64_t value, const COMPARISON_TYPE cmpType);
    void set(std::string &propertyPath, const FUNCTION_TYPE fType);

    bool equals(const AdaptiveIndexQuery &query) const;

    std::string toString() const {
        return "fnType: " + std::to_string(static_cast<double>(functionType)) + " contentType: " + std::to_string(static_cast<double>(contentType)) + " strVal: " + strValue + " cmpType: " + std::to_string(static_cast<double>(cmpType)) + " boolVal: " + std::to_string(boolVal) + " intVal: " + std::to_string(intVal);
    }
};