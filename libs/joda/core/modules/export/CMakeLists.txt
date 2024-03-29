project(JODA-Core-export CXX)

set(joda-core-export-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/export/IExportDestination.h
        ${CMAKE_CURRENT_LIST_DIR}/src/StorageExport.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/export/StorageExport.h
        ${CMAKE_CURRENT_LIST_DIR}/src/FileExport.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/export/FileExport.h
        ${CMAKE_CURRENT_LIST_DIR}/src/JoinExport.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/export/JoinExport.h
        ${CMAKE_CURRENT_LIST_DIR}/src/DirectoryExport.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/export/DirectoryExport.h
        ${CMAKE_CURRENT_LIST_DIR}/src/StreamExport.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/export/StreamExport.h
        ${CMAKE_CURRENT_LIST_DIR}/src/JSONStringifier.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/export/JSONStringifier.h
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-export-SOURCES} PARENT_SCOPE )
add_library(joda-core-export-lib INTERFACE)

target_include_directories(joda-core-export-lib
        INTERFACE ./include/
        )

target_link_libraries(joda-core-export-lib INTERFACE joda-core-concurrency-lib joda-core-pipelineatomics-lib)

#########################################################
#
# GLOG
#
#########################################################

target_link_libraries(joda-core-export-lib INTERFACE glog::glog)
