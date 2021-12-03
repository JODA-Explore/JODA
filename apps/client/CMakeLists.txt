project(joda_client VERSION 0.2.2 LANGUAGES CXX)

add_definitions(-DJODA_CLIENT_VERSION="${PROJECT_VERSION}")

add_executable(joda-client src/client.cpp)
target_link_libraries(joda-client PRIVATE joda-client-lib glog::glog)

if (NCURSES_SIMPLE_INCLUDE)
    target_compile_definitions(joda-client PRIVATE JODA_CURSES_SIMPLE_HEADER)
endif ()

#########################################################
#
# Install
#
#########################################################
install(TARGETS joda-client
        RUNTIME DESTINATION bin
        COMPONENT binaries)