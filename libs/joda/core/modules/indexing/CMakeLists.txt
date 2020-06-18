project(JODA-Core-indexing CXX)

set(joda-core-indexing-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/cache/CacheEntry.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/cache/QueryCache.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/cache/CacheEntry.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/indexing/QueryCache.h
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-indexing-SOURCES} PARENT_SCOPE )
add_library(joda-core-indexing-lib INTERFACE)

target_include_directories(joda-core-indexing-lib
        INTERFACE ./include/
        )

find_package(rapidjson REQUIRED)
target_include_directories(joda-core-indexing-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(joda-core-indexing-lib INTERFACE joda-core-document-lib joda-core-query-pred-lib)

#########################################################
#
# GLOG
#
#########################################################
find_package(glog REQUIRED)
target_link_libraries(joda-core-indexing-lib INTERFACE glog)