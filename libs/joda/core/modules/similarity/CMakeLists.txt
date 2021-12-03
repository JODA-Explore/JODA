project(JODA-Core-similarity CXX)


set(joda-core-similarity-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/similarity/measures/IJSONSimilarityMeasure.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/similarity/measures/PathJaccard.h
        ${CMAKE_CURRENT_LIST_DIR}/src/RJPathsReader.h
        ${CMAKE_CURRENT_LIST_DIR}/src/PathJaccard.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/RJPathsReader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/similarity/SimilarityScheduler.h
        ${CMAKE_CURRENT_LIST_DIR}/src/AttributeJaccard.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/similarity/measures/AttributeJaccard.h
        ${CMAKE_CURRENT_LIST_DIR}/src/RJAttributeReader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/RJAttributeReader.h
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-similarity-SOURCES} PARENT_SCOPE )

add_library(joda-core-similarity-lib INTERFACE )
target_include_directories(joda-core-similarity-lib
        INTERFACE ./include/
        )


find_package(rapidjson REQUIRED)
target_include_directories(joda-core-similarity-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(joda-core-similarity-lib
        INTERFACE
        joda-core-storage-lib
        )

#########################################################
#
# GLOG
#
#########################################################

target_link_libraries(joda-core-similarity-lib INTERFACE glog::glog)


