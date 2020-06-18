project(JODA-Core-query-values CXX)

set(joda-core-query-values-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/ArraySizeProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/FileNameProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/FilePosProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/IDProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/INListProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/IsXBoolProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/IValueProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ListAttributesProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/AtomProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/PointerProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/RegexExtractProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/RegexProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/RegexReplaceProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/SeqNumberProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/TypeProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/ArraySizeProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/BinaryNumberProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/UnaryNumberProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/UnaryStringProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/BinaryStringProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/FileNameProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/FilePosProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/IDProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/INListProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/IsXBoolProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/IValueProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/ListAttributesProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/AtomProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/PointerProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/RegexExtractProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/RegexProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/RegexReplaceProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/SeqNumberProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/TypeProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/src/NullProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/NullProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/values/ConstantNumber.h
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-query-values-SOURCES} PARENT_SCOPE )

add_library(joda-core-query-values-lib INTERFACE )

target_include_directories(joda-core-query-values-lib
        INTERFACE ./include/
        )


find_package(rapidjson REQUIRED)
target_include_directories(joda-core-query-values-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(joda-core-query-values-lib INTERFACE joda-core-document-lib joda-core-misc-lib)
target_link_libraries(joda-core-query-values-lib INTERFACE pegtl)
#########################################################
#
# GLOG
#
#########################################################
find_package(glog REQUIRED)
target_link_libraries(joda-core-query-values-lib INTERFACE glog)