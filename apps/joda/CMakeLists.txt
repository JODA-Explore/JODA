###########################################################
##############    JODA  ###################################
###########################################################
project(joda)


add_executable(joda joda.cpp)

target_link_libraries(joda PRIVATE joda-core-lib)


target_compile_options(joda PRIVATE "$<$<CONFIG:DEBUG>:${JODA_DEBUG_OPTIONS}>")
target_compile_options(joda PRIVATE "$<$<CONFIG:RELEASE>:${JODA_RELEASE_OPTIONS}>")
target_compile_options(joda PRIVATE "$<$<CONFIG:RELWITHDEBINFO>:${JODA_RELWITHDEBINFO_OPTIONS}>")


if (NCURSES_SIMPLE_INCLUDE)
    target_compile_definitions(joda PRIVATE JODA_CURSES_SIMPLE_HEADER)
endif ()

#########################################################
#
# Auto Alloc (opt)
#
#########################################################
if (JODA_ALLOC STREQUAL "AUTO")
    find_package(JeMalloc)
    if (JEMALLOC_FOUND)
        message("Linking JeMalloc")
        target_link_libraries(joda PRIVATE ${JEMALLOC_LIBRARIES})
        #set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
        #set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}  -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
    else()
        find_package(Gperftools)
        if (GPERFTOOLS_FOUND)
            message("Linking Tcmalloc")
            target_link_libraries(joda PRIVATE tcmalloc)
            #set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
            #set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}  -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
        endif()
    endif()
endif()

#########################################################
#
# JeMalloc (opt)
#
#########################################################
if (JODA_ALLOC STREQUAL "JEMALLOC")
    find_package(JeMalloc REQUIRED)
    message("Linking JeMalloc")
    target_link_libraries(joda PRIVATE ${JEMALLOC_LIBRARIES})
endif()
#########################################################
#########################################################

#########################################################
#
# TCMALLOC (opt)
#
#########################################################
if (JODA_ALLOC STREQUAL "TCMALLOC")
    find_package(Gperftools REQUIRED)
    message("Linking Tcmalloc")
    target_link_libraries(joda PRIVATE tcmalloc)
endif()
if (JODA_ALLOC STREQUAL "PROFILE")
    find_package(Gperftools REQUIRED)
    message("Linking Tcmalloc and Profiler")
    target_link_libraries(joda PRIVATE tcmalloc_and_profiler)
    target_compile_options(joda PRIVATE "-pg")
    #message("***Profiling Joda App ***")
    #target_compile_definitions(joda PUBLIC JODA_ENABLE_PROFILING)
endif()
#########################################################

#########################################################
#
# stdc++fs
#
#########################################################

target_link_libraries(joda PUBLIC stdc++fs)


#########################################################
#
# Install
#
#########################################################
install(TARGETS joda
        RUNTIME DESTINATION bin
        COMPONENT binaries)