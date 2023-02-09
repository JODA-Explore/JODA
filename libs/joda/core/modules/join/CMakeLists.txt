project(JODA-Core-join CXX)

set(joda-core-join-SOURCES
       ${CMAKE_CURRENT_LIST_DIR}/src/FileJoinManager.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/JoinManager.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/MemoryJoinDoc.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/MemoryJoinManager.cpp
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/join/FileJoinManager.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/join/JoinManager.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/join/MemoryJoinDoc.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/join/MemoryJoinManager.h
       ${CMAKE_CURRENT_LIST_DIR}/src/JoinParser.cpp
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/join/JoinParser.h
       ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/LoadJoin.cpp
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/join/pipeline/LoadJoin.h
       ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/StoreJoin.cpp
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/join/pipeline/StoreJoin.h
       ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/LoadJoinFiles.cpp
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/join/pipeline/LoadJoinFiles.h
       ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/LoadJoinFileParser.cpp
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/join/pipeline/LoadJoinFileParser.h
       # Actual join
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/join/relation/ContainerJoinExecutor.h
       ${CMAKE_CURRENT_LIST_DIR}/src/relation/ContainerJoinExecutor.cpp
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/join/relation/EqualityJoinExecutor.h
       ${CMAKE_CURRENT_LIST_DIR}/src/relation/EqualityJoinExecutor.cpp
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/join/relation/ThetaJoinExecutor.h
       ${CMAKE_CURRENT_LIST_DIR}/src/relation/ThetaJoinExecutor.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/InnerJoin.cpp
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/join/pipeline/InnerJoin.h
       ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/OuterJoin.cpp
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/join/pipeline/OuterJoin.h
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-join-SOURCES} PARENT_SCOPE )
add_library(joda-core-join-lib INTERFACE)

target_include_directories(joda-core-join-lib
        INTERFACE ./include/
        )


find_package(rapidjson REQUIRED)
target_include_directories(joda-core-join-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(joda-core-join-lib
        INTERFACE
        joda-core-query-values-lib
        joda-core-fs-lib
        joda-core-storage-lib
        joda-core-config-lib
        )
#########################################################
#
# GLOG
#
#########################################################

target_link_libraries(joda-core-join-lib INTERFACE glog::glog)