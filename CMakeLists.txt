cmake_minimum_required(VERSION 3.7.2 FATAL_ERROR)

project(clang_tool_experimental)

set(CMAKE_CXX_STANDARD 17)

find_package(Clang)

set(LLVM_LINK_COMPONENTS
        Support
        )

add_executable(print_template_specialization print_template_specialization.cpp)
target_link_libraries(print_template_specialization
        PRIVATE
        clangAST
        clangBasic
        clangFrontend
        clangSerialization
        clangTooling
        )

add_executable(print_arg_types print_arg_types.cpp)
target_link_libraries(print_arg_types
        PRIVATE
        clangAST
        clangBasic
        clangFrontend
        clangSerialization
        clangTooling
        )

add_executable(print_arg_types_via_matcher print_arg_types_via_matcher.cpp)
target_link_libraries(print_arg_types_via_matcher
        PRIVATE
        clangAST
        clangBasic
        clangFrontend
        clangSerialization
        clangTooling
        )

add_executable(shit_printer shit_printer.cpp)
target_link_libraries(shit_printer
        PRIVATE
        clangAST
        clangBasic
        clangFrontend
        clangSerialization
        clangTooling
        )

add_executable(sample sample.cpp)

set(copy_source_dir "${CMAKE_SOURCE_DIR}")
set(copy_dest_dir "${CMAKE_BINARY_DIR}")
set(copy_file_name "sample.cpp")

message(STATUS ${CMAKE_BINARY_DIR})

add_custom_command(
        TARGET print_template_specialization POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${copy_source_dir}/${copy_file_name}" "${copy_dest_dir}/${copy_file_name}"
)

add_custom_command(
        TARGET print_arg_types POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${copy_source_dir}/${copy_file_name}" "${copy_dest_dir}/${copy_file_name}"
)

add_custom_command(
        TARGET print_arg_types_via_matcher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${copy_source_dir}/${copy_file_name}" "${copy_dest_dir}/${copy_file_name}"
)

add_custom_command(
        TARGET shit_printer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${copy_source_dir}/${copy_file_name}" "${copy_dest_dir}/${copy_file_name}"
)