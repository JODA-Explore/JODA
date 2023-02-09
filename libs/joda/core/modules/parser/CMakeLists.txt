project(JODA-Core-parser CXX)

set(joda-core-parser-SOURCES
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/sources/JSONFileDirectorySource.cpp
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/sources/JSONFileSource.cpp
         ${CMAKE_CURRENT_LIST_DIR}/include/joda/parser/JSONPipelineTextParser.h
         ${CMAKE_CURRENT_LIST_DIR}/include/joda/parser/JSONPipelineStreamParser.h
         ${CMAKE_CURRENT_LIST_DIR}/include/joda/parser/ParserIntermediates.h
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/sources/JSONFileDirectorySource.cpp
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/sources/JSONFileSource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/readers/sources/JSONURLSource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/readers/sources/JSONInStreamSource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/parsers/ContainerScheduler/DefaultContainerScheduler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/parsers/ContainerScheduler/DefaultContainerScheduler.h
        # Pipeline
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/parser/pipeline/ListFile.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/ListFile.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/parser/pipeline/InStream.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/InStream.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/parser/pipeline/ListDir.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/ListDir.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/parser/pipeline/FileOpener.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/FileOpener.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/parser/pipeline/LSFileMapper.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/LSFileMapper.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/parser/pipeline/URLStream.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/URLStream.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/parser/pipeline/LineSeparatedStreamReader.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/LineSeparatedStreamReader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/parser/pipeline/TextParser.h
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-parser-SOURCES} PARENT_SCOPE )
add_library(joda-core-parser-lib INTERFACE )


target_include_directories(joda-core-parser-lib
        INTERFACE ./include/
        )

target_link_libraries(joda-core-parser-lib
        INTERFACE
        joda-core-concurrency-lib
        joda-core-config-lib
        joda-core-storage-lib
        joda-core-fs-lib
        joda-core-similarity-lib
        joda-core-misc-lib
        joda-core-pipelineatomics-lib
        )

find_package(rapidjson REQUIRED)
target_include_directories(joda-core-parser-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})

#########################################################
#
# GLOG
#
#########################################################

target_link_libraries(joda-core-parser-lib INTERFACE glog::glog)

if (BUILD_WEBIMPORT MATCHES ON)
    set(cpprestsdk_DIR /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}/cmake/)
    find_library(cpprestsdk-lib cpprest)
    if (NOT cpprestsdk-lib)
        message(ERROR " cpprestsdk library not found")
    else ()
        message("Compiling with FROM URL support")
        target_compile_definitions(joda-core-parser-lib INTERFACE JODA_ENABLE_FROMURL)
        target_link_libraries(joda-core-parser-lib INTERFACE ${cpprestsdk-lib})
        find_package(OpenSSL REQUIRED SSL)
        target_link_libraries(joda-core-parser-lib INTERFACE OpenSSL::SSL)
    endif ()
endif (BUILD_WEBIMPORT MATCHES ON)




