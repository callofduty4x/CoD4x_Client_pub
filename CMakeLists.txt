cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

enable_language(ASM)

enable_language(ASM_NASM)
if (WIN32)
    set(CMAKE_ASM_NASM_OBJECT_FORMAT win)
    set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} --prefix _")
endif()

project(cod4x_client)

# add_compile_options(-fmax-errors=5)

add_subdirectory(src/discord-rpc-api)
add_subdirectory(src/diskinfo)
add_subdirectory(src/libudis86)
add_subdirectory(src/exeobfus)
add_subdirectory(src/libseh-0.0.4)

# TODO TEMPORARY to get legacy compiled
include_directories(src)
add_subdirectory(src/mbedtls)
target_include_directories(mbedcrypto PRIVATE src)

add_subdirectory(src/steam_api)
add_subdirectory(src/tomcrypt)

# TODO TEMP to get legacy compiled
add_subdirectory(freetype-2.9)

add_subdirectory(cod4_guard/sys_dumper)

add_subdirectory(src)

add_subdirectory(CoD4x_ClientLoaderStub)
