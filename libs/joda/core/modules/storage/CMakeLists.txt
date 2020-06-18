add_subdirectory(document)
add_subdirectory(collection)
add_subdirectory(container)

set(joda-core-storage-SOURCES
         ${CMAKE_CURRENT_LIST_DIR}/include/joda/storage/JSONStorage.h
         ${CMAKE_CURRENT_LIST_DIR}/src/JSONStorage.cpp
        )


set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-storage-SOURCES} PARENT_SCOPE )

add_library(joda-core-storage-lib INTERFACE)

target_include_directories(joda-core-storage-lib
        INTERFACE ./include/
        )


find_package(rapidjson REQUIRED)
target_include_directories(joda-core-storage-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(joda-core-storage-lib
        INTERFACE
        joda-core-concurrency-lib
        joda-core-misc-lib
        joda-core-document-lib
        joda-core-container-lib
        joda-core-config-lib
        joda-core-fs-lib
        )
#########################################################
#
# GLOG
#
#########################################################
find_package(glog REQUIRED)
target_link_libraries(joda-core-storage-lib INTERFACE glog)