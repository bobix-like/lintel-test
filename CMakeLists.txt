cmake_minimum_required(VERSION 4.0)
project(Lintel-test-task CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(test)

add_custom_target(run_tests
    COMMAND ${CMAKE_BINARY_DIR}/test/test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}

    COMMENT "Running all tests"
)

