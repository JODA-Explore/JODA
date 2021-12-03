project(JODA-Core-query-proj CXX)



set(joda-core-query-proj-SOURCES
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/project/FlattenProjector.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/project/IProjector.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/project/ISetProjector.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/project/PointerCopyProject.h
       ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/project/ValueProviderProjector.h
       ${CMAKE_CURRENT_LIST_DIR}/src/PointerCopyProject.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/setprojection/FlattenProjector.cpp
       ${CMAKE_CURRENT_LIST_DIR}/src/ValueProviderProjector.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/DeletePointerProjector.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/query/project/DeletePointerProjector.h
        )
set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-query-proj-SOURCES} PARENT_SCOPE )
add_library(joda-core-query-proj-lib INTERFACE)

target_include_directories(joda-core-query-proj-lib
        INTERFACE ./include/
        )


find_package(rapidjson REQUIRED)
target_include_directories(joda-core-query-proj-lib INTERFACE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(joda-core-query-proj-lib INTERFACE joda-core-query-values-lib joda-core-document-lib)

#########################################################
#
# GLOG
#
#########################################################

target_link_libraries(joda-core-query-proj-lib INTERFACE glog::glog)