cmake_minimum_required(VERSION 3.1...3.28)

project(badiff_library VERSION 1.5.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(NOT CMAKE_VERSION VERSION_LESS 3.9)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT BADIFF_IPO_SUPPORTED OUTPUT BADIFF_IPO_ERROR)
  if(BADIFF_IPO_SUPPORTED)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
  else()
    message(STATUS "IPO/LTO not supported, continuing without it: ${BADIFF_IPO_ERROR}")
  endif()
endif()

option(BADIFF_NATIVE_ARCH "Optimize for the host CPU (-march=native)" OFF)
if(BADIFF_NATIVE_ARCH AND CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native" CACHE STRING "C++ compiler flags" FORCE)
endif()

option(BADIFF_ENABLE_TESTS "Enable test suite" ON)
if(BADIFF_ENABLE_TESTS)
enable_testing()
add_subdirectory(test)
endif()

## shared library

add_library(badiff_library SHARED
    src/empty.cpp
)
target_link_libraries(badiff_library PRIVATE -Wl,--whole-archive badiff_archive -Wl,--no-whole-archive)
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
    src/op.cpp
    src/alg/inertial_graph.cpp
    src/q/cdc_op_queue.cpp
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

set_target_properties(badiff_cli PROPERTIES OUTPUT_NAME "badiff")

install (TARGETS badiff_cli
    DESTINATION bin)

## man page

install(FILES ${PROJECT_SOURCE_DIR}/man/badiff.1 
    DESTINATION share/man/man1)


