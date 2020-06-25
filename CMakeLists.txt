cmake_minimum_required(VERSION 2.8)
project( AES )
add_executable( AES main.c )

add_library(aeslib STATIC
    lib/aes-intrinsics.c
    lib/aes.c
    lib/utils.c
)
set_target_properties(aeslib PROPERTIES COMPILE_FLAGS "-maes -mavx")
target_include_directories(aeslib PRIVATE include)

include_directories( include )
target_link_libraries( AES aeslib )