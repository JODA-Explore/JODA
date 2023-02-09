project(JODA-Core-network CXX)

set(joda-core-network-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/JodaServer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/network/JodaServer.h
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/JodaQueryRequest.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/JodaQueryRequest.h
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/JodaResultRequest.h
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/JodaResultRequest.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/JodaSourceRequest.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/JodaSourceRequest.h
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/DeleteRequest.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/DeleteRequest.h
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/SystemRequest.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/SystemRequest.h        
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/ModulePost.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/ModulePost.h
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/ModuleRequest.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/ModuleRequest.h
        ${CMAKE_CURRENT_LIST_DIR}/src/apiv2/API.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/network/apiv2/API.h
        ${CMAKE_CURRENT_LIST_DIR}/src/Favicon.h
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-network-SOURCES} PARENT_SCOPE)
add_library(joda-core-network-lib INTERFACE)

target_include_directories(joda-core-network-lib
        INTERFACE ./include/
        )



target_link_libraries(joda-core-network-lib
        INTERFACE
        joda-core-misc-lib
        joda-core-queryparsing-lib
        joda-core-queryexecution-lib
        joda-core-storage-collection-lib
        )
#########################################################
#
# GLOG
#
#########################################################

target_link_libraries(joda-core-network-lib INTERFACE glog::glog)

#########################################################
#
# RapidJSON
#
#########################################################
find_package(rapidjson REQUIRED)
target_include_directories(joda-core-network-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})

#########################################################
#
# Boost
#
#########################################################
find_package(Boost REQUIRED system)
target_link_libraries(joda-core-network-lib INTERFACE Boost::boost Boost::system)

#########################################################
#
# httplib
#
#########################################################
target_link_libraries(joda-core-network-lib INTERFACE httplib)