cmake_minimum_required(VERSION 3.5)
project(steam-api VERSION 1.0.0 LANGUAGES C CXX)
include_guard()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(external/libudis86)

add_library(steam-api STATIC)

target_sources(steam-api PRIVATE
    src/steam_api.cpp
    src/steam_interface.cpp
)

target_include_directories(steam-api PUBLIC
    include
)

target_compile_options(steam-api PRIVATE -m32 -std=c++14)

target_link_options(steam-api PRIVATE -m32)

target_link_libraries(steam-api PRIVATE
    udis86
)
