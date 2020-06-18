set(PEGTL_BUILD_TESTS OFF CACHE BOOL "Build test programs" FORCE)
set(PEGTL_BUILD_EXAMPLES OFF CACHE BOOL "Build example programs" FORCE)
add_subdirectory(PEGTL)
add_subdirectory(cpp-httplib)
