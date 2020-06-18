project(JODA-Core-config CXX)

set(joda-core-config-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/config/config.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/config/ConfigParser.h
        ${CMAKE_CURRENT_LIST_DIR}/src/config.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ConfigParser.cpp
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-config-SOURCES} PARENT_SCOPE)

add_library(joda-core-config-lib INTERFACE)

target_include_directories(joda-core-config-lib
        INTERFACE ./include/
        )

#########################################################
#
# Boost
#
#########################################################
find_package(Boost REQUIRED program_options)
target_link_libraries(joda-core-config-lib INTERFACE Boost::boost Boost::program_options )

#########################################################
#
# GLOG
#
#########################################################
find_package(glog REQUIRED)
target_link_libraries(joda-core-config-lib INTERFACE glog)