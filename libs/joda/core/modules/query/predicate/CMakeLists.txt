project(JODA-Core-query-pred CXX)



set(joda-core-query-pred-SOURCES
       ${CMAKE_CURRENT_LIST_DIR}/src/AndPredicate.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/ComparePredicate.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/EqualizePredicate.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/NegatePredicate.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/OrPredicate.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/Predicate.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/ValToPredicate.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/visitors/BloomAttributeVisitor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/visitors/AttributeVisitor.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/visitors/CopyPredicateVisitor.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/visitors/PredicateVisitor.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/visitors/StaticEvalVisitor.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/visitors/ToStringVisitor.cpp
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/predicate/AndPredicate.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/predicate/BloomAttributeVisitor.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/predicate/AttributeVisitor.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/predicate/ComparePredicate.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/predicate/CopyPredicateVisitor.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/predicate/EqualizePredicate.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/predicate/OrPredicate.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/predicate/Predicate.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/predicate/NegatePredicate.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/predicate/PredicateVisitor.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/predicate/StaticEvalVisitor.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/predicate/ToStringVisitor.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/predicate/ValToPredicate.h
        )

add_library(joda-core-query-pred-lib INTERFACE)

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-query-pred-SOURCES} PARENT_SCOPE )

target_include_directories(joda-core-query-pred-lib
        INTERFACE ./include/
        )


find_package(rapidjson REQUIRED)
target_include_directories(joda-core-query-pred-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})


target_link_libraries(joda-core-query-pred-lib INTERFACE joda-core-query-values-lib)

#########################################################
#
# GLOG
#
#########################################################
find_package(glog REQUIRED)
target_link_libraries(joda-core-query-pred-lib INTERFACE glog)