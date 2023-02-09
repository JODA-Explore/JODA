project(JODA-Core-extension CXX)

set(joda-core-extension-SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/extension/ModuleRegister.h
        ${CMAKE_CURRENT_LIST_DIR}/src/ModuleRegister.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/extension/ModuleExecutorStorage.h
        ${CMAKE_CURRENT_LIST_DIR}/src/ModuleExecutorStorage.cpp
        # --- Python
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/extension/python/PythonModule.h
        ${CMAKE_CURRENT_LIST_DIR}/src/python/PythonModule.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/extension/python/PythonHandler.h
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/extension/python/PythonAggregator.h
        ${CMAKE_CURRENT_LIST_DIR}/src/python/PythonAggregator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/extension/python/PythonValueProvider.h
        ${CMAKE_CURRENT_LIST_DIR}/src/python/PythonValueProvider.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/extension/python/PythonExecutor.h
        ${CMAKE_CURRENT_LIST_DIR}/src/python/PythonExecutor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/extension/python/PythonImportSource.h
        ${CMAKE_CURRENT_LIST_DIR}/src/python/PythonImportSource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/extension/python/PythonImportTask.h
        ${CMAKE_CURRENT_LIST_DIR}/src/python/PythonImportTask.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/joda/extension/python/PythonExport.h
        ${CMAKE_CURRENT_LIST_DIR}/src/python/PythonExport.cpp
)

set(JODA-CORE-SOURCE-FILES ${JODA-CORE-SOURCE-FILES} ${joda-core-extension-SOURCES} PARENT_SCOPE )

add_library(joda-core-extension-lib INTERFACE )

target_include_directories(joda-core-extension-lib
        INTERFACE ./include/
        )


#target_link_libraries(joda-core-extension-lib PUBLIC )
#########################################################
#
# GLOG
#
#########################################################
target_link_libraries(joda-core-extension-lib INTERFACE glog::glog)


#########################################################
#
# Python
#
#########################################################

if (BUILD_PYTHON MATCHES ON)
    find_package (Python3 COMPONENTS Interpreter Development.Embed)
    if (NOT Python3_FOUND)
        message(ERROR " python library not found")
    else ()
        message("Compiling with python support")
        target_compile_definitions(joda-core-extension-lib INTERFACE JODA_ENABLE_PYTHON)
        target_include_directories(joda-core-extension-lib INTERFACE ${Python3_INCLUDE_DIRS})
        target_link_libraries(joda-core-extension-lib INTERFACE ${Python3_LIBRARIES})
    endif ()
endif (BUILD_PYTHON MATCHES ON)

