#!/bin/sh

CXX=i686-w64-mingw32-gcc
FLAGS="-ansi -DSTRICT_ANSI=1 -m32 -std=c++11 -c -s -O0 -Wall -DLINUX_COMPILE"
$CXX $FLAGS "src/connection_win.cpp"
$CXX $FLAGS "src/discord_register_win.cpp" -Iinclude
$CXX $FLAGS "src/discord_rpc.cpp" -Iinclude
$CXX $FLAGS "src/rpc_connection.cpp" -Iinclude
$CXX $FLAGS "src/serialization.cpp" -Iinclude

/usr/bin/x86_64-w64-mingw32-ar cr ../../lib/libdiscord_rpc.a *.o

