project(JODA-Core-queryparsing CXX)

set(joda-core-queryparsing-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/actions/Actions.h
        ${CMAKE_CURRENT_LIST_DIR}/src/Control.h
        ${CMAKE_CURRENT_LIST_DIR}/src/FunctionWrapper.h
        ${CMAKE_CURRENT_LIST_DIR}/src/grammar/Agg.h
        ${CMAKE_CURRENT_LIST_DIR}/src/grammar/As.h
        ${CMAKE_CURRENT_LIST_DIR}/src/grammar/Choose.h
        ${CMAKE_CURRENT_LIST_DIR}/src/grammar/Grammar.h
        ${CMAKE_CURRENT_LIST_DIR}/src/grammar/Literals.h
        ${CMAKE_CURRENT_LIST_DIR}/src/grammar/Load.h
        ${CMAKE_CURRENT_LIST_DIR}/src/grammar/Store.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/queryparsing/QueryParser.h
        ${CMAKE_CURRENT_LIST_DIR}/src/states/States.h
        ${CMAKE_CURRENT_LIST_DIR}/src/QueryParser.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/actions/Choose_Actions.h
        ${CMAKE_CURRENT_LIST_DIR}/src/actions/As_Actions.h
        ${CMAKE_CURRENT_LIST_DIR}/src/actions/Agg_Actions.h
        ${CMAKE_CURRENT_LIST_DIR}/src/actions/Load_Actions.h
        ${CMAKE_CURRENT_LIST_DIR}/src/actions/Store_Actions.h
        ${CMAKE_CURRENT_LIST_DIR}/src/actions/Function_Actions.h
        ${CMAKE_CURRENT_LIST_DIR}/src/states/Load_State.h
        ${CMAKE_CURRENT_LIST_DIR}/src/states/Choose_State.h
        ${CMAKE_CURRENT_LIST_DIR}/src/states/As_State.h
        ${CMAKE_CURRENT_LIST_DIR}/src/states/Agg_State.h
        ${CMAKE_CURRENT_LIST_DIR}/src/states/Store_State.h
        ${CMAKE_CURRENT_LIST_DIR}/src/states/Function_State.h
        ${CMAKE_CURRENT_LIST_DIR}/src/states/Query_State.h
        ${CMAKE_CURRENT_LIST_DIR}/src/error/Load_error.h
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-queryparsing-SOURCES} PARENT_SCOPE )

add_library(joda-core-queryparsing-lib INTERFACE )


target_include_directories(joda-core-queryparsing-lib
        INTERFACE ./include/
        )

target_link_libraries(joda-core-queryparsing-lib
        INTERFACE
        joda-core-query-lib
        joda-core-storage-lib
        joda-core-storage-collection-lib
        joda-core-parser-lib
        joda-core-document-lib
        joda-core-export-lib
        pegtl
        )
#########################################################
#
# GLOG
#
#########################################################

target_link_libraries(joda-core-queryparsing-lib INTERFACE glog::glog)