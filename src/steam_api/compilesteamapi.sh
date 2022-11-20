#!/bin/sh

CXX=i686-w64-mingw32-g++
$CXX -c -s -O0 -Wall "steam_api.cpp"
$CXX -c -s -O0 -Wall "steam_interface.cpp"

i686-w64-mingw32-ar cr ../../lib/libsteam_integration.a *.o

