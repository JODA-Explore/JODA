project(JODA-Core-query CXX)

add_subdirectory(values)
add_subdirectory(aggregate)
add_subdirectory(predicate)
add_subdirectory(project)

set(joda-core-query-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/Query.h
        ${CMAKE_CURRENT_LIST_DIR}/src/Query.cpp
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-query-SOURCES} PARENT_SCOPE )

add_library(joda-core-query-lib INTERFACE )

target_include_directories(joda-core-query-lib
        INTERFACE ./include/
        )


find_package(rapidjson REQUIRED)
target_include_directories(joda-core-query-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(joda-core-query-lib
        INTERFACE
        joda-core-misc-lib
        joda-core-join-lib
        joda-core-parser-lib
        joda-core-query-values-lib
        joda-core-query-agg-lib
        joda-core-query-pred-lib
        joda-core-query-proj-lib
        joda-core-export-lib
        )

#########################################################
#
# GLOG
#
#########################################################

target_link_libraries(joda-core-query-lib INTERFACE glog::glog)