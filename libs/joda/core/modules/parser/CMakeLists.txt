project(JODA-Core-parser CXX)

set(joda-core-parser-SOURCES
         ${CMAKE_CURRENT_LIST_DIR}/src/ReaderParser.cpp
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/BeautifulJSONFileReader.cpp
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/JSONFileReader.cpp
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/sources/JSONFileDirectorySource.cpp
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/sources/JSONFileSource.cpp
         ${CMAKE_CURRENT_LIST_DIR}/src/parsers/JSONMetaParser.h
         ${CMAKE_CURRENT_LIST_DIR}/src/parsers/JSONTextParser.h
         ${CMAKE_CURRENT_LIST_DIR}/include/joda/parser/ReaderParser.h
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/BeautifulJSONFileReader.h
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/IReader.h
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/JSONFileReader.h
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/JSONURLReader.cpp
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/JSONURLReader.h
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/sources/JSONFileDirectorySource.cpp
         ${CMAKE_CURRENT_LIST_DIR}/src/readers/sources/JSONFileSource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/readers/sources/JSONURLSource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/parsers/JSONStreamParser.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/parsers/JSONStreamParser.h
        ${CMAKE_CURRENT_LIST_DIR}/src/parsers/ContainerScheduler/DefaultContainerScheduler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/parsers/ContainerScheduler/DefaultContainerScheduler.h
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
        )

find_package(rapidjson REQUIRED)
target_include_directories(joda-core-parser-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})

#########################################################
#
# GLOG
#
#########################################################
find_package(glog REQUIRED)
target_link_libraries(joda-core-parser-lib INTERFACE glog)

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




