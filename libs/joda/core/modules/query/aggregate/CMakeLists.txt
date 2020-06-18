project(JODA-Core-query-agg CXX)



set(joda-core-query-agg-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/AttributeStatAggregator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/DistinctAggregator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/aggregation/AttributeStatAggregator.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/aggregation/DistinctAggregator.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/aggregation/IAggregator.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/aggregation/NumberAggregator.h
        ${CMAKE_CURRENT_LIST_DIR}/src/CollectAggregator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/aggregation/CollectAggregator.h
        ${CMAKE_CURRENT_LIST_DIR}/src/GroupAggregator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/aggregation/GroupAggregator.h
        ${CMAKE_CURRENT_LIST_DIR}/src/CountAggregator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/aggregation/CountAggregator.h
        )
set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-query-agg-SOURCES} PARENT_SCOPE )

add_library(joda-core-query-agg-lib INTERFACE )


target_include_directories(joda-core-query-agg-lib
        INTERFACE ./include/
        INTERFACE ./src/
        )


find_package(rapidjson REQUIRED)
target_include_directories(joda-core-query-agg-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(joda-core-query-agg-lib INTERFACE joda-core-query-values-lib joda-core-concurrency-lib)

#########################################################
#
# GLOG
#
#########################################################
find_package(glog REQUIRED)
target_link_libraries(joda-core-query-agg-lib INTERFACE glog)