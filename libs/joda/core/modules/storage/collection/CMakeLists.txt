project(JODA-Core-storage-collection CXX)

set(joda-core-storage-collection-SOURCES
         ${CMAKE_CURRENT_LIST_DIR}/include/joda/storage/collection/StorageCollection.h
         ${CMAKE_CURRENT_LIST_DIR}/src/StorageCollection.cpp
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-storage-collection-SOURCES} PARENT_SCOPE )

add_library(joda-core-storage-collection-lib INTERFACE)

target_include_directories(joda-core-storage-collection-lib
        INTERFACE ./include/
        )


find_package(rapidjson REQUIRED)
target_include_directories(joda-core-storage-collection-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})


target_link_libraries(joda-core-storage-collection-lib INTERFACE joda-core-join-lib joda-core-storage-lib joda-core-config-lib)
#########################################################
#
# GLOG
#
#########################################################
find_package(glog REQUIRED)
target_link_libraries(joda-core-storage-collection-lib INTERFACE glog)