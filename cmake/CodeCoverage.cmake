# Coverage flags
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang")
    message(FATAL_ERROR "Not Implemented! Yet. Use GCC for coverage")
elseif(CMAKE_COMPILER_IS_GNUCXX)

    find_program(GCOV_EXECUTABLE gcov)
    find_program(GCOVR_EXECUTABLE gcovr)

    if(NOT GCOV_EXECUTABLE)
        message(FATAL_ERROR "Gcov not found.")
    elseif(NOT GCOVR_EXECUTABLE)
        message(FATAL_ERROR "Gcovr not found.")
    endif()

    SET(COVERAGE_FLAGS --coverage ) #-fprofile-abs-path
    SET(COVERAGE_LIBS --coverage )

    SET(COVERAGE_DIR ${PROJECT_BINARY_DIR}/coverage)

    SET(PEPARE_COV "mkdir -p ${COVERAGE_DIR}")
    SET(COV_CMD "${GCOVR_EXECUTABLE} -r ${PROJECT_SOURCE_DIR} --exclude-unreachable-branches -d -j 4 -e extern/ -s  --html-details ${COVERAGE_DIR}/coverage.html --xml ${COVERAGE_DIR}/coverage.xml --gcov-executable ${GCOV_EXECUTABLE} ${PROJECT_BINARY_DIR}/libs ${PROJECT_BINARY_DIR}/apps ${PROJECT_BINARY_DIR}/test")

    configure_file( "${CMAKE_SOURCE_DIR}/test/coverage.sh.in"
            "${CMAKE_BINARY_DIR}/test/coverage.sh"
            )

else()
    message(FATAL_ERROR "Coverage needs GCC or Clang")
endif()
