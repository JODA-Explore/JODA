project(joda-client-lib)

set(client-SOURCES
        src/JodaClient.cpp
        src/QueryRequest.cpp src/QueryRequest.h src/ResultRequest.cpp src/ResultRequest.h)

add_library(joda-client-lib STATIC ${client-SOURCES})
target_include_directories(joda-client-lib
        PUBLIC include
        PRIVATE src ${CMAKE_SOURCE_DIR}/extern/rapidjson/include
        )

target_compile_options(joda-client-lib PRIVATE ${COVERAGE_FLAGS})
target_link_libraries(joda-client-lib PRIVATE ${COVERAGE_LIBS})
target_link_libraries(joda-client-lib PRIVATE -static-libstdc++)
find_package(Threads REQUIRED)
target_link_libraries(joda-client-lib PRIVATE Threads::Threads stdc++fs)

target_link_libraries(joda-client-lib PRIVATE glog::glog)

find_package(Boost REQUIRED system program_options)
target_link_libraries(joda-client-lib PUBLIC Boost::boost PRIVATE Boost::system Boost::program_options)

if (NCURSES_SIMPLE_INCLUDE)
    target_compile_definitions(joda-client-lib PRIVATE JODA_CURSES_SIMPLE_HEADER)
endif ()

find_package(Curses REQUIRED)
target_link_libraries(joda-client-lib PRIVATE ${CURSES_NCURSES_LIBRARY})


#########################################################
#
# httplib
#
#########################################################
target_link_libraries(joda-client-lib PUBLIC httplib)

#########################################################
#
# RapidJSON
#
#########################################################
find_package(rapidjson REQUIRED)
target_include_directories(joda-client-lib PUBLIC ${RAPIDJSON_INCLUDE_DIRS})