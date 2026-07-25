find_package(GTest REQUIRED)

add_executable(cpp_concurrency_in_action_tests
    test_greeting.cpp)

target_link_libraries(cpp_concurrency_in_action_tests
    PRIVATE
        cpp_concurrency_in_action::cpp_concurrency_in_action
        GTest::gtest_main
        project_warnings)

include(GoogleTest)
gtest_discover_tests(cpp_concurrency_in_action_tests)
