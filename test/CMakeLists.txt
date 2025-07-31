find_package(GTest REQUIRED)

add_executable(test test.cpp)

target_link_libraries(test PRIVATE GTest::gtest_main s1ap_db)

include(GoogleTest)
gtest_discover_tests(test)

