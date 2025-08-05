add_library(badiff_c_archive STATIC
    src/op.c
    src/op_q.c
)

target_include_directories(badiff_c_archive PUBLIC include)
