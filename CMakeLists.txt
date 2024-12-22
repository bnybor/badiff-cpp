cmake_minimum_required(VERSION 3.5)

project(badiff_library VERSION 1.0.0)

enable_testing()

set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" CACHE STRING "Set C++ Compiler Flags" FORCE)

add_subdirectory(test)

add_library(badiff_library SHARED 
    src/op.cpp
    src/badiff.cpp
    src/q/compacting_op_queue.cpp
    src/q/filter_op_queue.cpp
    src/q/replace_op_queue.cpp
    src/q/stream_replace_op_queue.cpp
    src/q/deserialize_op_queue.cpp
    src/q/vector_op_queue.cpp
    src/q/op_queue.cpp
    src/q/graph_op_queue.cpp
    src/q/minimize_op_queue.cpp
    src/q/coalescing_op_queue.cpp
    src/q/chunking_op_queue.cpp
    src/alg/inertial_graph.cpp
    src/alg/edit_graph.cpp
)
add_library(badiff::library ALIAS badiff_library)
set_target_properties(badiff_library PROPERTIES OUTPUT_NAME "badiff")

target_include_directories(badiff_library
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

add_executable(badiff_cli
    main/badiff_main.cpp
)

# link the new hello_library target with the hello_binary target
target_link_libraries( badiff_cli
    PRIVATE 
        badiff::library
)

set_target_properties(badiff_cli PROPERTIES OUTPUT_NAME "badiff")


