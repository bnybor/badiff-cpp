project(badiff_test)

add_executable(${PROJECT_NAME}
    src/badiff_test.cpp
    src/op_test.cpp
    src/alg/edit_graph_test.cpp
    src/alg/inertial_graph_test.cpp
    src/q/coalescing_op_queue_test.cpp
    src/q/graph_op_queue_test.cpp
    src/q/minimize_op_queue_test.cpp
    src/q/op_queue_test.cpp
    src/q/replace_op_queue_test.cpp
    src/q/rop_queue_test.cpp
    src/q/rreplace_op_queue_test.cpp
    src/q/rstream_replace_op_queue_test.cpp
    src/q/stream_replace_op_queue_test.cpp
    contrib/src/gmock/gmock-all.cc
    contrib/src/gtest/gtest-all.cc
    contrib/src/gtest/gtest_main.cc
)

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/contrib/include
)


target_link_libraries(${PROJECT_NAME}
    badiff_library
)

add_test(badiff_test badiff_test)
