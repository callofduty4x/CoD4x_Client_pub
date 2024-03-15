#!/bin/sh

[ -f build/Makefile ] || \
    cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc \
    -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ \
    -DOFFICIAL_BUILD=ON

cmake --build build --parallel $(nproc)
