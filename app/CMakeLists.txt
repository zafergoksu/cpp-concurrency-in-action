file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/app/*.cpp")

foreach(source_file IN LISTS SOURCES)
    get_filename_component(target_name ${source_file} NAME_WE)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} PRIVATE cpp_concurrency_in_action)
endforeach()
