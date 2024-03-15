cmake_minimum_required(VERSION 3.10)
project(udis86 VERSION 1.0.0 LANGUAGES C)
include_guard()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(udis86 STATIC
    src/decode.c
    src/itab.c
    src/syn.c
    src/syn-intel.c
    src/udis86.c
)

target_include_directories(udis86 PUBLIC
    include
)

target_compile_options(udis86 PRIVATE -m32 -std=c11)
target_link_options(udis86 PRIVATE -m32)

