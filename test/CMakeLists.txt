add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "extern/googletest")

mark_as_advanced(
        BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
        gmock_build_tests gtest_build_samples gtest_build_tests
        gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)


file(COPY data DESTINATION ${PROJECT_BINARY_DIR}/test/)

macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} gtest.cpp ${ARGN})
    target_compile_options(${TESTNAME} PRIVATE ${JODA_TEST_OPTIONS})
    target_compile_definitions(${TESTNAME} PRIVATE ${RAPIDJSON_COMPILE_DEFINITIONS})
    target_link_libraries(${TESTNAME} PRIVATE -Wl,--whole-archive joda-core-lib -Wl,--no-whole-archive)
    target_link_libraries(${TESTNAME} PRIVATE gtest gmock gtest_main glog::glog -static-libstdc++)
    target_compile_options(${TESTNAME} PRIVATE ${COVERAGE_FLAGS})
    target_link_libraries(${TESTNAME} PRIVATE ${COVERAGE_LIBS})
    set_target_properties(${TESTNAME} PROPERTIES FOLDER test)

    if (NCURSES_SIMPLE_INCLUDE)
        target_compile_definitions(${TESTNAME} PRIVATE JODA_CURSES_SIMPLE_HEADER)
    endif ()


    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME} WORKING_DIRECTORY ${PROJECT_BINARY_DIR} )
endmacro()

################################################################
###############   Tests   ######################################
################################################################

package_add_test(QueryParsing systemtests/QueryParsingTest.cpp ../libs/joda/core/modules/query/predicate/src/visitors/test/StaticEvalVisitorTest.cpp)
package_add_test(FullStack systemtests/FullStack.cpp)
package_add_test(FullStackNoStore systemtests/FullStack.cpp)
target_compile_definitions(FullStackNoStore PRIVATE JODA_TEST_NOSTORE_MODE) #Test nostore mode
package_add_test(ValueTest ../libs/joda/core/modules/query/values/test/IValueTestHelper.h ../libs/joda/core/modules/query/values/test/ValueTest.cpp)
package_add_test(AggregatorTest ../libs/joda/core/modules/query/aggregate/test/AggregatorTest.cpp ../libs/joda/core/modules/query/aggregate/test/AggregatorParsingTest.cpp ../libs/joda/core/modules/query/aggregate/test/NumberAggregatorTest.cpp)
package_add_test(SimilarityTest ../libs/joda/core/modules/similarity/test/RJPathTest.cpp ../libs/joda/core/modules/similarity/test/RJAttributeTest.cpp ../libs/joda/core/modules/similarity/test/MeasureTest.cpp)
package_add_test(ViewTest ../libs/joda/core/modules/storage/document/test/ViewTest.cpp ../libs/joda/core/modules/storage/document/test/VirtualObjectTest.cpp)
package_add_test(DirectoryFileScannerTest ../libs/joda/core/modules/filesystem/test/DirectoryFileScannerTest.cpp)
package_add_test(ServerTest ../libs/joda/core/modules/network/test/ServerTest.cpp ../libs/joda/core/modules/network/test/API2Test.cpp)
package_add_test(StorageTest ../libs/joda/core/modules/storage/collection/test/CollectionTest.cpp)

################################################################
###############   Coverage   ######################################
################################################################

if(CODE_COVERAGE MATCHES ON)
    add_test(NAME Coverage
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            COMMAND ${PROJECT_BINARY_DIR}/test/coverage.sh)
endif(CODE_COVERAGE MATCHES ON)