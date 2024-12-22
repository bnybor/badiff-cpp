project(badiff_test)

# Create the executable
add_executable(${PROJECT_NAME}
    src/badiff_test.cpp
    src/q/coalescing_op_queue_test.cpp
    src/q/op_queue_test.cpp
    src/q/graph_op_queue_test.cpp
    src/alg/edit_graph_test.cpp
    src/alg/inertial_graph_test.cpp
    src/op_test.cpp
    contrib/src/gmock/gmock-all.cc
    contrib/src/gtest/gtest-all.cc
    contrib/src/gtest/gtest_main.cc
)

target_include_directories(badiff_test
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/contrib/include
)


# Link the static library from subproject1 using its alias sub::lib1
# Link the header only library from subproject2 using its alias sub::lib2
# This will cause the include directories for that target to be added to this project
target_link_libraries(${PROJECT_NAME}
    badiff::library
)
