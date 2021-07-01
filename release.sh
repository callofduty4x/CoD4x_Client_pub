#!/bin/bash

if [ ! -f lib/libsteam_integration.a ]; then
	echo Compiling Steam API
	pushd src/steam_api || exit 1
	./compilesteamapi.sh
	echo Steam API Compiled!
    popd || exit 1
fi


if [ ! -f lib/libtomcrypt.a ]; then
	echo Compiling TomCrypt Library
	pushd src/tomcrypt || exit 1
	./compile.sh
	echo TomCrypt Compiled!
	popd || exit 1
fi

if [ ! -f lib/libdiscord_rpc.a ]; then
	echo Compiling Discord-RPC Library
	pushd src/discord-rpc-api || exit 1
	./compilediscordrpc.sh
	echo Discord-RPC Compiled!
    popd || exit 1
fi

if [ ! -f lib/libmbedtls_win32.a ]; then
	echo Compiling mbedtls Library
	pushd src/mbedtls || exit 1
	./build.sh
	echo mbedtls Compiled!
    popd || exit 1
fi

if [ ! -f lib/libudis86.a ]; then
	echo Compiling UDis86 Library
	pushd src/libudis86 || exit 1
	./compilelibudis86.sh
	echo UDis86 Compiled!
    popd || exit 1
fi

#if [ -f lib/libfreetype.a ]; then
#	echo Compiling FreeType Library
#	pushd freetype-2.9
#	mingw32-make
#	move objs/freetype.a ../lib/libfreetype.a
#	echo FreeType Compiled!
#    popd
#fi

./compiledll_common.sh

