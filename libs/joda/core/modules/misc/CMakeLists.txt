project(JODA-Core-misc CXX)

set(joda-core-misc-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/Benchmark.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/MemoryUtility.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/RecurringTimer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/Timer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/misc/Benchmark.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/misc/infix_iterator.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/misc/MemoryUtility.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/misc/RecurringTimer.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/misc/RJFwd.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/misc/Timer.h
        ${CMAKE_CURRENT_LIST_DIR}/src/JSONFileWriter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/misc/JSONFileWriter.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/misc/FileNameRepo.h
        ${CMAKE_CURRENT_LIST_DIR}/src/FileNameRepo.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/misc/ProgressBar.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/misc/OSUtility.h
        ${CMAKE_CURRENT_LIST_DIR}/src/OSUtility.cpp
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-misc-SOURCES} PARENT_SCOPE )
add_library(joda-core-misc-lib INTERFACE )

target_include_directories(joda-core-misc-lib
        INTERFACE ./include/
        )


find_package(rapidjson REQUIRED)
target_include_directories(joda-core-misc-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})

#########################################################
#
# GLOG
#
#########################################################

target_link_libraries(joda-core-misc-lib INTERFACE glog::glog)
