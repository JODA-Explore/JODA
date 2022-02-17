################################################################
###############   CMAKE Config   ###############################
################################################################
cmake_minimum_required(VERSION 3.14.0)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckCXXCompilerFlag)
INCLUDE(cmake/Distro.cmake)
system_info(DISTRO)


#Sanitizers
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=leak  -fsanitize=undefined -fPIE -pie")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=thread  -fsanitize=undefined -fPIE -pie")
################################################################
################   JODA Config   ###############################
################################################################
set(JODA_ALLOC "AUTO") #Possible values: JEMALLOC, TCMALLOC, AUTO , PROFILE
set(PACKAGE_AUTHOR "Nico Schäfer")

project(JODA VERSION 0.13.1 HOMEPAGE_URL https://joda-explore.github.io/JODA LANGUAGES CXX)

SET(JODA_API_VERSION 2)

#Git Revision
IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
    FIND_PACKAGE(Git)
    IF (GIT_FOUND)
        EXECUTE_PROCESS(
                COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                OUTPUT_VARIABLE "JODA_BUILD_VERSION"
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        MESSAGE(STATUS "Git version: ${JODA_BUILD_VERSION}")
    ELSE (GIT_FOUND)
        SET(JODA_BUILD_VERSION 0)
    ENDIF (GIT_FOUND)
ENDIF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
#Build time
string(TIMESTAMP JODA_BUILD_TIME "%H:%M:%S %d-%m-%Y")


configure_file( "${CMAKE_SOURCE_DIR}/libs/joda/core/include/joda/version.h.in"
        "${CMAKE_SOURCE_DIR}/libs/joda/core/include/joda/version.h"
        )



#########################################################
#
# Options
#
#########################################################
set(RAPIDJSON_COMPILE_DEFINITIONS)

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(JODA_DEBUG_OPTIONS -O0 -Wall)
set(JODA_RELEASE_OPTIONS -O2)
set(JODA_RELWITHDEBINFO_OPTIONS -O2)
set(JODA_TEST_OPTIONS ${JODA_DEBUG_OPTIONS})

option(ENABLE_PROFILING "Enable Profiling" OFF)
if (ENABLE_PROFILING MATCHES ON)
    message("****Profiling enabled****")
    set(JODA_ALLOC "PROFILE")
endif (ENABLE_PROFILING MATCHES ON)

option(CODE_COVERAGE "Build Coverage" OFF)
if(CODE_COVERAGE MATCHES ON)
    message("****Code Coverage enabled****")
    include(CodeCoverage)
endif(CODE_COVERAGE MATCHES ON)



option(BUILD_DOCS "Build Documentation" OFF)
if(BUILD_DOCS MATCHES ON)
    message("****Documentation enabled****")
    include(docs)
endif(BUILD_DOCS MATCHES ON)

if (BUILD_MARCH)
    set(INTERNAL_BUILD_MARCH ${BUILD_MARCH})
else ()
    set(INTERNAL_BUILD_MARCH "native")
endif ()

option(BUILD_ARCH "Build Architecture specific" OFF)
CHECK_CXX_COMPILER_FLAG("-march=${INTERNAL_BUILD_MARCH}" COMPILER_SUPPORTS_MARCH)
if (COMPILER_SUPPORTS_MARCH AND BUILD_ARCH MATCHES ON)
    message("****Compiling for Architecture****")
    set(JODA_RELEASE_OPTIONS ${JODA_RELEASE_OPTIONS} -march=${INTERNAL_BUILD_MARCH} -mtune=${INTERNAL_BUILD_MARCH})
    set(JODA_RELWITHDEBINFO_OPTIONS ${JODA_RELWITHDEBINFO_OPTIONS} -march=${INTERNAL_BUILD_MARCH} -mtune=${INTERNAL_BUILD_MARCH})
    cmake_host_system_information(RESULT SSE2_SUPPORTED QUERY HAS_SSE2)
    if (SSE2_SUPPORTED MATCHES 1)
        message("****Enable SSE2 support****")
        set(JODA_RELEASE_OPTIONS ${JODA_RELEASE_OPTIONS} -msse2)
        set(JODA_RELWITHDEBINFO_OPTIONS ${JODA_RELWITHDEBINFO_OPTIONS} -msse2)
        set(RAPIDJSON_USE_SSE2 ON)
        set(RAPIDJSON_COMPILE_DEFINITIONS ${RAPIDJSON_COMPILE_DEFINITIONS} RAPIDJSON_SSE2) #Enable std::string compatability
    endif (SSE2_SUPPORTED MATCHES 1)
endif (COMPILER_SUPPORTS_MARCH AND BUILD_ARCH MATCHES ON)

option(BUILD_WEBIMPORT "Build support for FROM URL" ON)
if (BUILD_WEBIMPORT MATCHES ON)
    message("****Web import enabled****")
endif (BUILD_WEBIMPORT MATCHES ON)


#########################################################
#
# Git Submodules
#
#########################################################
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/googletest/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

#########################################################
#
# Boost
#
#########################################################
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

#########################################################
#
# NCurses
#
#########################################################
if (DISTRO MATCHES "Arch Linux")
    message("Arch Linux, using simple ncurses")
    set(NCURSES_SIMPLE_INCLUDE 1)
else ()
    SET(CURSES_NEED_WIDE TRUE) #UTF-8
    SET(CURSES_NEED_NCURSES TRUE)
endif ()


#########################################################
#
# RapidJSON
#
#########################################################
set(RAPIDJSON_COMPILE_DEFINITIONS ${RAPIDJSON_COMPILE_DEFINITIONS} RAPIDJSON_HAS_STDSTRING=1) #Enable std::string compatability
set(RAPIDJSON_INCLUDEDIR "${CMAKE_CURRENT_LIST_DIR}/extern/rapidjson/include")

#########################################################
#
# Libraries
#
#########################################################
add_subdirectory(extern)


#########################################################
#
# Main
#
#########################################################

add_subdirectory(docs)
add_subdirectory(libs)
add_subdirectory(apps)

#########################################################
#
# Testing
#
#########################################################
option(BUILD_TESTS "Build tests" OFF)
if (BUILD_TESTS MATCHES ON)
    message("****Testing enabled****")
    enable_testing()
    add_subdirectory(test)
endif (BUILD_TESTS MATCHES ON)


#########################################################
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")


set(CPACK_PACKAGE_NAME "JODA")
set(CPACK_PACKAGE_VENDOR "Nico Schäfer")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "JSON On Demand Analysis Tool")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "JODA")
set(CPACK_PACKAGE_CONTACT "Nico Schäfer <nschaefer@cs.uni-kl.de>")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS  ON)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL binaries)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)

# This must always be last!
include(CPack)
