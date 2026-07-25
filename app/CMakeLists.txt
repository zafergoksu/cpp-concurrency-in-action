add_executable(cpp-concurrency-in-action
    main.cpp)

target_link_libraries(cpp-concurrency-in-action
    PRIVATE
        cpp_concurrency_in_action::cpp_concurrency_in_action
        project_warnings)
