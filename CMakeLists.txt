cmake_minimum_required(VERSION 3.15)
set(CMAKE_SYSTEM_NAME Windows)
project(CoD4X-client VERSION 1.0.0 LANGUAGES ASM_NASM C CXX)

option(OFFICIAL_BUILD "Build official release" OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(cod4x SHARED
    src/r_init.c
    src/stringed_hooks.c
    src/bg_items.c
    src/rb_backend.c
    src/cl_main.c
    src/pmove.c
    src/xasset_loader.c
    src/xassets/font.c
    src/xassets/material.c
    src/gfxcmds.c
    src/developer_utils.c
    src/cg_weapons.c
    src/files.c
    src/cl_rank.c
    src/com_stringtable.c
    src/phys.c
    src/com_findxassets.c
    src/win_gqos.c
    src/sec_common.c
    src/sec_crypto.c
    src/sec_init.c
    src/sec_sign.c
    src/sec_rsa_functions.c
    src/net_reliabletransport.c
    src/ui/cod4xupdate_pop.menu.c
    src/ui/cod4x_auconfirm.menu.c
    src/cl_console.c
    src/cl_keys.c
    src/r_main.c
    src/xassets.c
    src/cg_servercmds.c
    src/cg_main.c
    src/cg_font.c
    src/cg_scoreboard.c
    src/win_main.c
    src/sys_net.c
    src/httpftp.c
    src/snd_system.c
    src/cl_ui.c
    src/ui_main.c
    src/qshared.c
    src/bg_shared.c
    src/q_math.c
    src/server.c
    src/other.c
    src/cvar.c
    src/cl_netchan.c
    src/cmd.c
    src/common.c
    src/msg.c
    src/huffman.c
    src/g_team.c
    src/net_chan.c
    src/sys_patch.c
    src/sv_main.c
    src/sv_game.c
    src/sv_world.c
    src/win_input.c
    src/win_wndproc.c
    src/win_syscon.c
    src/win_sys.c
    src/punkbuster_interface.c
    src/md5.c
    src/null.c
    src/crc.c
    src/dobj.c
    src/cod4x_unzip_utils.c

    src/callbacks.asm
    src/client_callbacks.asm
    src/mss32jumptable.asm
    src/fsdword.asm
)

if(OFFICIAL_BUILD)
    add_subdirectory(external/client-auth)
    target_compile_definitions(cod4x PRIVATE OFFICIAL=1)
endif()

target_include_directories(cod4x PRIVATE src)
set_target_properties(cod4x PROPERTIES OUTPUT_NAME "cod4x_021")

add_subdirectory(external/discord-rpc)
add_subdirectory(external/mbedtls)
add_subdirectory(external/steam_api)
add_subdirectory(external/tomcrypt)
add_subdirectory(external/unzip)

target_link_libraries(cod4x PRIVATE
    discord-rpc
    mbedtls
    steam-api
    tomcrypt
    unzip

    version
    kernel32
    advapi32
    ole32
    oleaut32
    uuid
    wsock32
    ws2_32
    dnsapi
    winmm
    shell32
    user32
    gdi32
    crypt32
    d3dx9_34
    psapi
    msvcr100
)

set(C_FLAGS -m32 -O3 -std=c11)
set(CXX_FLAGS -m32 -O3 -std=c++17)
set(NASM_FLAGS -f win32 --prefix _ -Ox)

target_compile_options(cod4x PRIVATE
    $<$<COMPILE_LANGUAGE:C>:${C_FLAGS}>
    $<$<COMPILE_LANGUAGE:CXX>:${CXX_FLAGS}>
    $<$<COMPILE_LANGUAGE:ASM_NASM>:${NASM_FLAGS}>
)

set_source_files_properties(src/sys_net.c PROPERTIES COMPILE_FLAGS -O1) # FIXME: awaiting connection on `-O` higher than -O1
set_source_files_properties(src/cl_ui.c PROPERTIES COMPILE_FLAGS -O0) # FIXME: glitching server refresh on `-O` higher than -O0

target_link_options(cod4x PRIVATE -m32 -static -Wl,--exclude-libs,msvcrt.a,-fPic,--stack,8388608)

target_compile_definitions(cod4x PRIVATE
    MAX_LOCAL_CLIENTS=1
)

