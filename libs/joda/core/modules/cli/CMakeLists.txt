project(JODA-core-cli CXX)

set(joda-core-cli-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/CLI.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/CLICompletion.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/cli/CLI.h
        ${CMAKE_CURRENT_LIST_DIR}/src/CLICompletion.h
        ${CMAKE_CURRENT_LIST_DIR}/src/ResultInterface.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ResultInterface.h
        ${CMAKE_CURRENT_LIST_DIR}/src/JSONInterface.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/JSONInterface.h
        ${CMAKE_CURRENT_LIST_DIR}/src/CursesHandler.h
        )


set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-cli-SOURCES} PARENT_SCOPE )

add_library(joda-core-cli-lib INTERFACE )

target_include_directories(joda-core-cli-lib
        INTERFACE include
        )

#########################################################
#
# Readline
#
#########################################################
find_package(Readline REQUIRED )
target_link_libraries(joda-core-cli-lib INTERFACE ${Readline_LIBRARY})

find_package(Curses REQUIRED)
target_link_libraries(joda-core-cli-lib INTERFACE ${CURSES_NCURSES_LIBRARY})

#target_link_libraries(joda-core-cli-lib
#        INTERFACE
#        joda-core-lib
#)
