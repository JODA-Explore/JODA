project(JODA-Core-queryexecution CXX)

set(joda-core-queryexecution-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/CacheExecutor.h
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/ConstExecutor.h
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/DefaultExecutor.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/queryexecution/IQueryExecutor.h
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/CacheExecutor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/ConstExecutor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/DefaultExecutor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/AdaptiveIndexExecutor.h
        ${CMAKE_CURRENT_LIST_DIR}/src/executor/AdaptiveIndexExecutor.cpp

        # Query Plan
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/queryexecution/PipelineQueryPlan.h
        ${CMAKE_CURRENT_LIST_DIR}/src/PipelineQueryPlan.cpp
        # Pipeline
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/Pipeline.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/Pipeline.cpp
        # -- Optimization
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/optimization/OptimizationRule.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/optimization/OptimizationRule.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/optimization/QueryCombinationRule.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/optimization/QueryCombinationRule.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/optimization/FilterAggregationRule.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/optimization/FilterAggregationRule.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/optimization/SimpleOptimizationRule.h
        # -- Optimization Tasks
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/optimization/tasks/ChooseAsPipeline.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/optimization/tasks/ChooseAsPipeline.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/optimization/tasks/ChooseAsAggPipeline.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/optimization/tasks/ChooseAsAggPipeline.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/optimization/tasks/ChooseAggPipeline.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/optimization/tasks/ChooseAggPipeline.h
        # -- Storage
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/storage/StorageBuffer.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/storage/StorageBuffer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/storage/StorageReceiver.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/storage/StorageReceiver.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/storage/StorageSender.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/storage/StorageSender.cpp
        # -- CHOOSE
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/choose/ChoosePipeline.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/choose/ChoosePipeline.cpp
        # -- AS
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/as/AsPipeline.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/as/AsPipeline.cpp
        # -- AGG
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/agg/AggPipeline.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/agg/AggPipeline.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/agg/FilterAggPipeline.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/agg/FilterAggPipeline.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/agg/AggMerger.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/agg/AggMerger.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/agg/WindowAgg.h
        ${CMAKE_CURRENT_LIST_DIR}/src/pipeline/tasks/agg/WindowAgg.cpp

        )

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-queryexecution-SOURCES} PARENT_SCOPE )

add_library(joda-core-queryexecution-lib INTERFACE)

target_include_directories(joda-core-queryexecution-lib
        INTERFACE ./include/
        )



target_link_libraries(joda-core-queryexecution-lib
        INTERFACE
        joda-core-query-lib
        joda-core-concurrency-lib
        joda-core-config-lib
        joda-core-indexing-lib
        joda-core-storage-lib
        joda-core-pipelineatomics-lib
        joda-core-extension-lib
)
#########################################################
#
# GLOG
#
#########################################################

target_link_libraries(joda-core-queryexecution-lib INTERFACE glog::glog)

