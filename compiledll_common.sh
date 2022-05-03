binfilename=cod4x_021

CC=i686-w64-mingw32-gcc
CXX=i686-w64-mingw32-g++
FLAGS="-m32 -s -Wall -O0 -DNDEBUG"
FLAGS_O3="-m32 -s -O3 -Wall -DNDEBUG"

bindir=out/bin/cod4x_021
[ -d $bindir ] || mkdir -p $bindir

cd bin

$CC -c $FLAGS_O3 -DNOUNCRYPT ../src/unzip/*.c
$CC -c $FLAGS "../src/r_init.c"
$CC -c $FLAGS "../src/stringed_hooks.c"
$CC -c $FLAGS "../src/bg_items.c"
$CC -c $FLAGS "../src/rb_backend.c"
$CC -c $FLAGS "../src/cl_main.c"
$CC -c $FLAGS "../src/pmove.c"
$CC -c $FLAGS "../src/xasset_loader.c"
$CC -c $FLAGS "../src/xassets/font.c" -I../freetype-2.9/include
$CC -c $FLAGS "../src/xassets/material.c"
$CC -c $FLAGS "../src/gfxcmds.c"
$CC -c $FLAGS "../src/developer_utils.c"
$CC -c $FLAGS "../src/cg_weapons.c"

$CC -c $FLAGS "../src/files.c"
$CC -c $FLAGS "../src/cl_rank.c"
$CC -c $FLAGS "../src/com_stringtable.c"
$CC -c $FLAGS_O3 "../src/phys.c"
$CC -c $FLAGS "../src/com_findxassets.c"
$CC -c $FLAGS "../src/win_gqos.c"
$CC -c $FLAGS "../src/sec_common.c"
$CC -c $FLAGS "../src/sec_crypto.c"
$CC -c $FLAGS "../src/sec_init.c"
$CC -c $FLAGS "../src/sec_sign.c"
$CC -c $FLAGS "../src/sec_rsa_functions.c"

$CC -c $FLAGS "../src/net_reliabletransport.c"
$CC -c $FLAGS "../src/ui/cod4xupdate_pop.menu.c"
$CC -c $FLAGS "../src/ui/cod4x_auconfirm.menu.c"
$CC -c $FLAGS "../src/cl_console.c"
$CC -c $FLAGS "../src/cl_keys.c"
$CC -c $FLAGS "../src/r_main.c"
$CC -c $FLAGS "../src/xassets.c"
$CC -c $FLAGS "../src/cg_servercmds.c"
$CC -c $FLAGS "../src/cg_main.c"
$CC -c $FLAGS "../src/cg_font.c"
$CC -c $FLAGS "../src/cg_scoreboard.c"
$CC -c $FLAGS "../src/win_main.c"
$CC -c $FLAGS "../src/sys_net.c"
$CC -c $FLAGS "../src/httpftp.c"

$CC -c $FLAGS "../src/snd_system.c"
$CC -c $FLAGS "../src/cl_ui.c"
$CC -c $FLAGS "../src/ui_main.c"
$CC -c $FLAGS "../src/qshared.c"
$CC -c $FLAGS "../src/q_math.c"
$CC -c $FLAGS "../src/server.c"
$CC -c $FLAGS "../src/other.c"
$CC -c $FLAGS "../src/cvar.c"
$CC -c $FLAGS "../src/cl_netchan.c"
$CC -c $FLAGS "../src/cmd.c"
$CC -c $FLAGS_O3 "../src/common.c"
$CC -c $FLAGS "../src/msg.c"
$CC -c $FLAGS "../src/huffman.c"
$CC -c $FLAGS "../src/g_team.c"
$CC -c $FLAGS "../src/net_chan.c"
$CC -c $FLAGS "../src/sys_patch.c"
$CC -c $FLAGS "../src/sv_main.c"
$CC -c $FLAGS "../src/sv_game.c"
$CC -c $FLAGS "../src/sv_world.c"
$CC -c $FLAGS "../src/win_input.c"
$CC -c $FLAGS "../src/win_wndproc.c"
$CC -c $FLAGS "../src/win_syscon.c"
$CC -c $FLAGS "../src/win_sys.c"
$CC -c $FLAGS "../src/punkbuster_interface.c"
$CC -c $FLAGS "../src/md5.c"
$CC -c $FLAGS "../src/null.c"
$CC -c $FLAGS "../src/crc.c"
$CC -c $FLAGS "../src/dobj.c"

echo Compiling NASM...
nasm -f win32 ../src/callbacks.asm --prefix _ -o callbacks.o
nasm -f win32 ../src/client_callbacks.asm --prefix _ -o client_callbacks.o
nasm -f win32 ../src/mss32jumptable.asm --prefix _ -o mss32jumptable.o
nasm -f win32 ../src/fsdword.asm --prefix _ -o fsdword.o

cd "../"
echo Linking...

$CXX -shared $FLAGS -o "out/bin/${binfilename}.dll" bin/*.o -Llib -lsteam_integration -ludis86 -ltomcrypt -lmbedtls_win32 -ldiscord_rpc -static -lversion -lkernel32 -ladvapi32 -lole32 -loleaut32 -luuid -lwsock32 -lws2_32 -lwinmm -lshell32 -luser32 -lgdi32 -lcrypt32 -ld3dx9_34 -lpsapi -lmsvcr100 -Wl,--exclude-libs,msvcrt.a,-fPic,--stack,8388608

