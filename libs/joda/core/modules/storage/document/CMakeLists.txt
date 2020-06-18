project(JODA-Core-document CXX)

set(joda-core-document-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/origin/FileOrigin.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/document/FileOrigin.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/document/IOrigin.h
        ${CMAKE_CURRENT_LIST_DIR}/src/origin/TemporaryOrigin.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/document/TemporaryOrigin.h
        ${CMAKE_CURRENT_LIST_DIR}/src/RapidJsonDocument.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/document/RapidJsonDocument.h
        ${CMAKE_CURRENT_LIST_DIR}/src/origin/URLOrigin.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/document/URLOrigin.h
        ${CMAKE_CURRENT_LIST_DIR}/src/origin/IDPositionOrigin.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/document/IDPositionOrigin.h
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-document-SOURCES} PARENT_SCOPE )

add_library(joda-core-document-lib INTERFACE)

target_include_directories(joda-core-document-lib
        INTERFACE ./include/
        )


find_package(rapidjson REQUIRED)
target_include_directories(joda-core-document-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(joda-core-document-lib INTERFACE joda-core-misc-lib)
#########################################################
#
# GLOG
#
#########################################################
find_package(glog REQUIRED)
target_link_libraries(joda-core-document-lib INTERFACE glog)