project(JODA-Core-indexing CXX)

set(joda-core-indexing-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/cache/CacheEntry.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/cache/QueryCache.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/cache/CacheEntry.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/indexing/QueryCache.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/environment/ManagedDataType.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/environment/ManagedIndexData.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/environment/ManagedIndexData.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/environment/DataContext.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/environment/DataContext.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/AdaptiveIndexQuery.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/indexing/AdaptiveIndexQuery.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/indexing/AdaptiveIndexManager.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/AdaptiveIndexManager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/indexing/AdaptiveQueryExecutorVisitor.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/indexing/AdaptiveIndexQueryTreeVisitor.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/util/UnionTreeNodeDocumentHelper.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/util/UnionTreeNodeDocumentHelper.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/UnionTreeNode.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/UnionTreeNode.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/AdaptiveHistogram.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/AdaptiveHistogram.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/NumberIndex.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/NumberIndex.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/AdaptiveTrie.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/AdaptiveTrie.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/environment/MemoryManager.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/environment/MemoryManager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/util/AdaptiveQueryCreator.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/util/AdaptiveQueryCreator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/visitors/NextPredicateVisitor.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/visitors/NextPredicateVisitor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/visitors/AdaptiveQueryVisitor.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/visitors/AdaptiveQueryExecutorVisitor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/visitors/SetPredicateVisitor.h
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/visitors/SetPredicateVisitor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/adaptive/visitors/AdaptiveIndexQueryTreeVisitor.cpp
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-indexing-SOURCES} PARENT_SCOPE )
add_library(joda-core-indexing-lib INTERFACE)

target_include_directories(joda-core-indexing-lib
        INTERFACE ./include/
        )

find_package(rapidjson REQUIRED)
target_include_directories(joda-core-indexing-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(joda-core-indexing-lib INTERFACE joda-core-document-lib)

#########################################################
#
# GLOG
#
#########################################################

target_link_libraries(joda-core-indexing-lib INTERFACE glog::glog)