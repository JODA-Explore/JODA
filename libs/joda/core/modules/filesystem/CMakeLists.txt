project(JODA-Core-fs CXX)

set(joda-core-fs-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/fs/DirectoryFileScanner.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/fs/DirectoryRegister.h
        ${CMAKE_CURRENT_LIST_DIR}/src/DirectoryFileScanner.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/DirectoryRegister.cpp
        )

add_library(joda-core-fs-lib INTERFACE )

target_include_directories(joda-core-fs-lib
        INTERFACE ./include/
        )


set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-fs-SOURCES} PARENT_SCOPE )


target_link_libraries(joda-core-fs-lib INTERFACE  joda-core-config-lib)
#########################################################
#
# GLOG
#
#########################################################

target_link_libraries(joda-core-fs-lib INTERFACE glog::glog)