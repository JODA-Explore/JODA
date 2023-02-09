project(JODA-Core-pipelineatomics CXX)

set(joda-core-pipelineatomics-SOURCES
        # Pipeline Queue
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/pipelineatomics/queue/PipelineQueue.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/pipelineatomics/queue/PipelineIOQueue.h
        # Pipeline Task
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/pipelineatomics/PipelineTask.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/pipelineatomics/PipelineIOTask.h
        # Executor
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/pipelineatomics/ExecutorTemplate.h
        # TaskID
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/pipelineatomics/TaskID.h
        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-pipelineatomics-SOURCES} PARENT_SCOPE )

add_library(joda-core-pipelineatomics-lib INTERFACE )

target_include_directories(joda-core-pipelineatomics-lib
        INTERFACE ./include/
        )


target_link_libraries(joda-core-pipelineatomics-lib
        INTERFACE
        joda-core-misc-lib
)
#########################################################
#
# GLOG
#
#########################################################
target_link_libraries(joda-core-pipelineatomics-lib INTERFACE glog::glog)
