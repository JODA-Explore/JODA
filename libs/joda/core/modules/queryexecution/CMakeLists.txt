project(JODA-Core-queryexecution CXX)

set(joda-core-queryexecution-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/CacheExecutor.h
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/ConstExecutor.h
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/DefaultExecutor.h
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/IQueryExecutor.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/queryexecution/QueryPlan.h
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/CacheExecutor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/ConstExecutor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/DefaultExecutor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/QueryPlan.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/QueryThread.h
        ${CMAKE_CURRENT_LIST_DIR}/src/QueryThread.cpp
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-queryexecution-SOURCES} PARENT_SCOPE )

add_library(joda-core-queryexecution-lib INTERFACE)

target_include_directories(joda-core-queryexecution-lib
        INTERFACE ./include/
        )



target_link_libraries(joda-core-queryexecution-lib
        INTERFACE
        joda-core-query-lib
        joda-core-concurrency-lib
        joda-core-config-lib
        joda-core-indexing-lib
        joda-core-storage-lib
)
#########################################################
#
# GLOG
#
#########################################################
find_package(glog REQUIRED)
target_link_libraries(joda-core-queryexecution-lib INTERFACE glog)
