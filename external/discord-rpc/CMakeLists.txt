cmake_minimum_required(VERSION 3.5)
project(discord-rpc VERSION 1.0.0 LANGUAGES CXX)
include_guard()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(discord-rpc STATIC)

target_sources(discord-rpc PRIVATE
    src/connection_win.cpp
    src/discord_register_win.cpp
    src/discord_rpc.cpp
    src/rpc_connection.cpp
    src/serialization.cpp
)

target_include_directories(discord-rpc
    PUBLIC include
    PRIVATE include/discord-rpc
)

target_compile_definitions(discord-rpc PRIVATE
    STRICT_ANSI=1
)

target_compile_options(discord-rpc PRIVATE
    -ansi
    -m32
    -s
    -std=c++14
)

target_link_options(discord-rpc PRIVATE
    -export-all-symbols
    -m32
)

