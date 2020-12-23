@echo off
@set binfilename=cod4x_019

set bindir=%LOCALAPPDATA%\CallofDuty4MW
if not exist %bindir% (
    mkdir %bindir%
)

set bindir=%bindir%\bin
if not exist %bindir% (
    mkdir %bindir%
)

set bindir=%bindir%\%binfilename%
if not exist %bindir% (
    mkdir %bindir%
)


echo.
echo Compiling...

cd bin

gcc.exe -c %FLAGS_O3% -D NOUNCRYPT "../src/unzip/*.c"
gcc.exe -c %FLAGS% "../src/r_init.c"
gcc.exe -c %FLAGS% "../src/stringed_hooks.c"
gcc.exe -c %FLAGS% "../src/bg_items.c"
gcc.exe -c %FLAGS% "../src/rb_backend.c"
gcc.exe -c %FLAGS% "../src/cl_main.c"
gcc.exe -c %FLAGS% "../src/pmove.c"
gcc.exe -c %FLAGS% "../src/xasset_loader.c"
gcc.exe -c %FLAGS% "../src/xassets/font.c" -I../freetype-2.9/include
gcc.exe -c %FLAGS% "../src/xassets/material.c"
gcc.exe -c %FLAGS% "../src/gfxcmds.c"
gcc.exe -c %FLAGS% "../src/developer_utils.c"
gcc.exe -c %FLAGS% "../src/cg_weapons.c"

::g++.exe -s -fno-strict-aliasing -c -O3 "../src/antireversing/antireversing.c"



gcc.exe -c %FLAGS% "../src/files.c"
gcc.exe -c %FLAGS% "../src/cl_rank.c"
gcc.exe -c %FLAGS% "../src/com_stringtable.c"
gcc.exe -c %FLAGS_O3% "../src/phys.c"
gcc.exe -c %FLAGS% "../src/com_findxassets.c"
gcc.exe -c %FLAGS% "../src/win_gqos.c"
gcc.exe -c %FLAGS% "../src/sec_common.c"
gcc.exe -c %FLAGS% "../src/sec_crypto.c"
gcc.exe -c %FLAGS% "../src/sec_init.c"
gcc.exe -c %FLAGS% "../src/sec_sign.c"
gcc.exe -c %FLAGS% "../src/sec_rsa_functions.c"

gcc.exe -c %FLAGS% "../src/net_reliabletransport.c"
gcc.exe -c %FLAGS% "../src/ui/cod4xupdate_pop.menu.c"
gcc.exe -c %FLAGS% "../src/ui/cod4x_auconfirm.menu.c"
REM gcc.exe -c %FLAGS% "../src/xasset_dump.c"
REM gcc.exe -c %FLAGS% "../src/menu_ccode_generator.c"
REM gcc.exe -c %FLAGS% "../src/parse\l_memory.c"
REM gcc.exe -c %FLAGS% "../src/parse\l_log.c"
REM gcc.exe -c %FLAGS% -D SCREWUP "../src/parse\l_precomp.c"
REM gcc.exe -c %FLAGS% -D SCREWUP "../src/parse\l_script.c"
gcc.exe -c %FLAGS% "../src/cl_console.c"
gcc.exe -c %FLAGS% "../src/cl_keys.c"
gcc.exe -c %FLAGS% "../src/r_main.c"
gcc.exe -c %FLAGS% "../src/xassets.c"
gcc.exe -c %FLAGS% "../src/cg_servercmds.c"
gcc.exe -c %FLAGS% "../src/cg_main.c"
gcc.exe -c %FLAGS% "../src/cg_font.c"
gcc.exe -c %FLAGS% "../src/cg_scoreboard.c"
gcc.exe -c %FLAGS% "../src/win_main.c"
gcc.exe -c %FLAGS% "../src/sys_net.c"
gcc.exe -c %FLAGS% "../src/httpftp.c"

:: -D NOANTIREVERSING

gcc.exe -c %FLAGS% "../src/snd_system.c"
gcc.exe -c %FLAGS% "../src/cl_ui.c"
gcc.exe -c %FLAGS% "../src/ui_main.c"
gcc.exe -c %FLAGS% "../src/qshared.c"
gcc.exe -c %FLAGS% "../src/q_math.c"
gcc.exe -c %FLAGS% "../src/server.c"
gcc.exe -c %FLAGS% "../src/other.c"
gcc.exe -c %FLAGS% "../src/cvar.c"
gcc.exe -c %FLAGS% "../src/cl_netchan.c"
gcc.exe -c %FLAGS% "../src/cmd.c"
gcc.exe -c %FLAGS_O3% "../src/common.c"
gcc.exe -c %FLAGS% "../src/msg.c"
gcc.exe -c %FLAGS% "../src/huffman.c"
gcc.exe -c %FLAGS% "../src/g_team.c"
gcc.exe -c %FLAGS% "../src/net_chan.c"
gcc.exe -c %FLAGS% "../src/sys_patch.c"
gcc.exe -c %FLAGS% "../src/sv_main.c"
gcc.exe -c %FLAGS% "../src/sv_game.c"
gcc.exe -c %FLAGS% "../src/sv_world.c"
gcc.exe -c %FLAGS% "../src/win_input.c"
gcc.exe -c %FLAGS% "../src/win_wndproc.c"
gcc.exe -c %FLAGS% "../src/win_syscon.c"
gcc.exe -c %FLAGS% "../src/win_sys.c"
gcc.exe -c %FLAGS% "../src/punkbuster_interface.c"
gcc.exe -c %FLAGS% "../src/md5.c"
gcc.exe -c %FLAGS% "../src/null.c"
gcc.exe -c %FLAGS% "../src/crc.c"
gcc.exe -c %FLAGS% "../src/dobj.c"

echo Compiling NASM...
::echo %CD%
::pause
nasm -f win32 ../src/callbacks.asm --prefix _ -o callbacks.o
nasm -f win32 ../src/client_callbacks.asm --prefix _ -o client_callbacks.o
nasm -f win32 ../src/mss32jumptable.asm --prefix _ -o mss32jumptable.o
nasm -f win32 ../src/fsdword.asm --prefix _ -o fsdword.o

cd "../"
echo Linking...
g++.exe -shared %FLAGS% -o bin/%binfilename%.dll bin/*.o -Llib/ -lsteam_integration -ludis86 -ltomcrypt -lmbedtls_win32 -ldiscord_rpc -static -lversion -lkernel32 -ladvapi32 -lole32 -loleaut32 -luuid -lwsock32 -lws2_32 -lwinmm -lshell32 -luser32 -lgdi32 -lcrypt32 -ld3dx9_34 -lpsapi -lfreetype -lmsvcr100 -Wl,--exclude-libs,msvcrt.a,-Map=%bindir%\%binfilename%.map,-fPic,--stack,8388608



echo Cleaning Up...
cd bin
REM del "*.o"


move %binfilename%.dll %bindir%\%binfilename%.dll

cd ..