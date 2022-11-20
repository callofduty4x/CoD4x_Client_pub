#!/bin/sh

CXX=i686-w64-mingw32-gcc

$CXX -m32 -c -s -O0 -Wall "decode.c"
$CXX -m32 -c -s -O0 -Wall "itab.c"
$CXX -m32 -c -s -O0 -std=gnu99 -Wall "syn.c"
$CXX -m32 -c -s -O0 -Wall "syn-intel.c"
$CXX -m32 -c -s -O0 -Wall "udis86.c"

i686-w64-mingw32-ar cr ../../lib/libudis86.a *.o
rm *.o

