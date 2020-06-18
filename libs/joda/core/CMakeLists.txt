project(JODA-Core CXX)

add_subdirectory(modules/config)
add_subdirectory(modules/parser)
add_subdirectory(modules/concurrency)
add_subdirectory(modules/misc)
add_subdirectory(modules/storage)
add_subdirectory(modules/indexing)
add_subdirectory(modules/query)
add_subdirectory(modules/join)
add_subdirectory(modules/filesystem)
add_subdirectory(modules/queryparsing)
add_subdirectory(modules/queryexecution)
add_subdirectory(modules/network)
add_subdirectory(modules/cli)
add_subdirectory(modules/export)
add_subdirectory(modules/similarity)



set(joda-core-SOURCES
        include/joda/version.h
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-SOURCES})

add_library(joda-core-lib STATIC ${JODA-CORE-SOURCE-FILES})

target_link_libraries(joda-core-lib PRIVATE -static-libstdc++)

target_compile_options(joda-core-lib PRIVATE ${COVERAGE_FLAGS})
target_link_libraries(joda-core-lib PRIVATE ${COVERAGE_LIBS})

target_compile_options(joda-core-lib PRIVATE "$<$<CONFIG:DEBUG>:${JODA_DEBUG_OPTIONS}>")
target_compile_options(joda-core-lib PRIVATE "$<$<CONFIG:RELEASE>:${JODA_RELEASE_OPTIONS}>")
target_compile_options(joda-core-lib PRIVATE "$<$<CONFIG:RELWITHDEBINFO>:${JODA_RELWITHDEBINFO_OPTIONS}>")


if (NCURSES_SIMPLE_INCLUDE)
    target_compile_definitions(joda-core-lib PRIVATE JODA_CURSES_SIMPLE_HEADER)
endif ()

target_compile_definitions(joda-core-lib PRIVATE ${RAPIDJSON_COMPILE_DEFINITIONS})


target_include_directories(joda-core-lib
        PUBLIC ./include/
        PRIVATE ./src/
        )

target_link_libraries(joda-core-lib
        PUBLIC
        joda-core-config-lib
        joda-core-parser-lib
        joda-core-concurrency-lib
        joda-core-misc-lib
        joda-core-storage-lib
        joda-core-storage-collection-lib
        joda-core-indexing-lib
        joda-core-query-lib
        joda-core-join-lib
        joda-core-fs-lib
        joda-core-queryparsing-lib
        joda-core-queryexecution-lib
        joda-core-network-lib
        joda-core-cli-lib
        joda-core-export-lib
        )



#########################################################
#
# Threads
#
#########################################################

find_package(Threads REQUIRED)
target_link_libraries(joda-core-lib PUBLIC Threads::Threads)

#########################################################
#
# stdc++fs
#
#########################################################

target_link_libraries(joda-core-lib PUBLIC stdc++fs)

if (JODA_ALLOC STREQUAL "PROFILE")
    find_package(Gperftools REQUIRED)
    target_compile_definitions(joda-core-lib PUBLIC JODA_ENABLE_PROFILING)
endif ()