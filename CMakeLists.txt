cmake_minimum_required(VERSION 3.5)

project(badiff_library VERSION 1.2.2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto -funroll-all-loops -fopenmp -march=native" CACHE STRING "Set C++ Compiler Flags" FORCE)
endif()

enable_testing()
add_subdirectory(test)

add_subdirectory(badiff_c)

## shared library

add_library(badiff_minimal_library SHARED 
    src/minimal.cpp
    src/op.cpp
    src/alg/inertial_graph.cpp
    src/q/compacting_op_queue.cpp
    src/q/filter_op_queue.cpp
    src/q/minimize_op_queue.cpp
    src/q/op_queue.cpp
    src/q/vector_op_queue.cpp
)
set_target_properties(badiff_minimal_library PROPERTIES OUTPUT_NAME "badiff_minimal")

target_include_directories(badiff_minimal_library
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

install (TARGETS badiff_minimal_library
    LIBRARY DESTINATION lib)

## static library

add_library(badiff_minimal_archive STATIC 
    src/minimal.cpp
    src/op.cpp
    src/alg/inertial_graph.cpp
    src/q/compacting_op_queue.cpp
    src/q/filter_op_queue.cpp
    src/q/minimize_op_queue.cpp
    src/q/op_queue.cpp
    src/q/vector_op_queue.cpp
)
set_target_properties(badiff_minimal_archive PROPERTIES OUTPUT_NAME "badiff_minimal")

target_include_directories(badiff_minimal_archive
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

install (TARGETS badiff_minimal_archive
    LIBRARY DESTINATION lib)

## shared library

add_library(badiff_library SHARED 
    src/badiff.cpp
    src/minimal.cpp
    src/op.cpp
    src/alg/edit_graph.cpp
    src/alg/inertial_graph.cpp
    src/q/chunking_op_queue.cpp
    src/q/coalescing_op_queue.cpp
    src/q/compacting_op_queue.cpp
    src/q/deserialize_op_queue.cpp
    src/q/filter_op_queue.cpp
    src/q/graph_op_queue.cpp
    src/q/minimize_op_queue.cpp
    src/q/op_queue.cpp
    src/q/replace_op_queue.cpp
    src/q/rewinding_op_queue.cpp
    src/q/stream_replace_op_queue.cpp
    src/q/vector_op_queue.cpp
)
set_target_properties(badiff_library PROPERTIES OUTPUT_NAME "badiff")

target_include_directories(badiff_library
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

install (TARGETS badiff_library
    LIBRARY DESTINATION lib)

## static library

add_library(badiff_archive STATIC 
    src/badiff.cpp
    src/minimal.cpp
    src/op.cpp
    src/alg/edit_graph.cpp
    src/alg/inertial_graph.cpp
    src/q/chunking_op_queue.cpp
    src/q/coalescing_op_queue.cpp
    src/q/compacting_op_queue.cpp
    src/q/deserialize_op_queue.cpp
    src/q/filter_op_queue.cpp
    src/q/graph_op_queue.cpp
    src/q/minimize_op_queue.cpp
    src/q/op_queue.cpp
    src/q/replace_op_queue.cpp
    src/q/rewinding_op_queue.cpp
    src/q/stream_replace_op_queue.cpp
    src/q/vector_op_queue.cpp
)
set_target_properties(badiff_archive PROPERTIES OUTPUT_NAME "badiff")

target_include_directories(badiff_archive
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

install (TARGETS badiff_archive
    LIBRARY DESTINATION lib)

## headers

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/badiff 
    DESTINATION include/)

## cli

add_executable(badiff_cli
    main/badiff_main.cpp
)

target_link_libraries( badiff_cli
    PRIVATE 
        badiff_archive
)

## cli, only here to validate that minimal library compiles

add_executable(badiff_minimal_cli
    main/minimal_main.cpp
)

target_link_libraries( badiff_minimal_cli
    PRIVATE 
        badiff_minimal_archive
)

set_target_properties(badiff_cli PROPERTIES OUTPUT_NAME "badiff")

install (TARGETS badiff_cli
    DESTINATION bin)

## man page

install(FILES ${PROJECT_SOURCE_DIR}/man/badiff.1 
    DESTINATION share/man/man1)


