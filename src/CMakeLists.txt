add_library(cpp_concurrency_in_action
    ScopedThread.cpp
    JoiningThread.cpp
    ThreadSafeStack.cpp)
add_library(cpp_concurrency_in_action::cpp_concurrency_in_action ALIAS cpp_concurrency_in_action)

target_include_directories(cpp_concurrency_in_action
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>)

target_link_libraries(cpp_concurrency_in_action PRIVATE project_warnings)
