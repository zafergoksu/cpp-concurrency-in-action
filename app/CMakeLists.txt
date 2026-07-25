add_executable(hello_world
    hello_world.cpp)

target_link_libraries(hello_world
    PRIVATE
        cpp_concurrency_in_action::cpp_concurrency_in_action
        project_warnings)
