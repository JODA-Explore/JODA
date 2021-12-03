project(JODA-Core-container CXX)

set(joda-core-container-SOURCES
         ${CMAKE_CURRENT_LIST_DIR}/include/joda/container/ContainerFlags.h
         ${CMAKE_CURRENT_LIST_DIR}/include/joda/container/JSONContainer.h
         ${CMAKE_CURRENT_LIST_DIR}/src/JSONContainer.cpp
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-container-SOURCES} PARENT_SCOPE )

add_library(joda-core-container-lib INTERFACE )

target_include_directories(joda-core-container-lib
        INTERFACE ./include/
        )


target_link_libraries(joda-core-container-lib
        INTERFACE
        joda-core-misc-lib
        joda-core-document-lib
        joda-core-query-proj-lib
        joda-core-indexing-lib
        joda-core-config-lib
        )
#########################################################
#
# GLOG
#
#########################################################

target_link_libraries(joda-core-container-lib INTERFACE glog::glog)