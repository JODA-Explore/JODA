project(JODA-Core-concurrency CXX)

set(joda-core-concurrency-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/ThreadManager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/concurrency/atomicops.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/concurrency/blockingconcurrentqueue.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/concurrency/concurrentqueue.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/concurrency/readerwriterqueue.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/concurrency/IOThreadPool.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/concurrency/IThreadUser.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/concurrency/ThreadManager.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/concurrency/ThreadQueueUser.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/concurrency/Queue.h)

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-concurrency-SOURCES} PARENT_SCOPE )

add_library(joda-core-concurrency-lib INTERFACE)

target_include_directories(joda-core-concurrency-lib
        INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include/
        )


#########################################################
#
# GLOG
#
#########################################################
target_link_libraries(joda-core-concurrency-lib INTERFACE glog::glog)