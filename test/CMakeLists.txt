add_executable(badiff_test
    src/badiff_test.cpp
    src/minimal_test.cpp
    src/op_test.cpp
    src/alg/edit_graph_test.cpp
    src/alg/inertial_graph_test.cpp
    src/q/coalescing_op_queue_test.cpp
    src/q/graph_op_queue_test.cpp
    src/q/minimize_op_queue_test.cpp
    src/q/op_queue_test.cpp
    src/q/replace_op_queue_test.cpp
    src/q/stream_replace_op_queue_test.cpp

    src/c/diff_patch_test.cpp

    contrib/src/gmock/gmock-all.cc
    contrib/src/gtest/gtest-all.cc
    contrib/src/gtest/gtest_main.cc
)

target_include_directories(badiff_test
    PUBLIC 
        include
    PRIVATE
        contrib/include
)


target_link_libraries(badiff_test
    badiff_library
    badiff_c_archive
)

add_test(badiff_test badiff_test)
