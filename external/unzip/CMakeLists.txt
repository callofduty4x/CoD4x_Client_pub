cmake_minimum_required(VERSION 3.5)
project(unzip VERSION 1.0.0 LANGUAGES C)
include_guard()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(unzip STATIC)

target_sources(unzip PRIVATE
    src/adler32.c
    src/crc32.c
    src/deflate.c
    src/inffast.c
    src/inflate.c
    src/inftrees.c
    src/trees.c
    src/unzip.c
)

target_include_directories(unzip
    PRIVATE include/unzip
    PUBLIC include
)

target_compile_options(unzip PRIVATE -m32 -std=c11)
target_link_options(unzip PRIVATE -m32)

