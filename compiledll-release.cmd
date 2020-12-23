@echo off

if not exist lib\libsteam_integration.a (
	echo Compiling Steam API
	cd src/steam_api
	call compilesteamapi.cmd
	echo Steam API Compiled!
	cd ../../
)


if not exist lib\libtomcrypt.a (
	echo Compiling TomCrypt Library
	cd src\tomcrypt
	call compile.cmd
	echo TomCrypt Compiled!
	cd ../../
)

if not exist lib\libdiscord_rpc.a (
	echo Compiling Discord-RPC Library
	cd src\discord-rpc-api
	call compilediscordrpc.cmd
	echo Discord-RPC Compiled!
	cd ../../
)

if not exist lib\libmbedtls_win32.a (
	echo Compiling mbedtls Library
	cd src\mbedtls
	call build.cmd
	echo mbedtls Compiled!
	cd ../../
)

if not exist lib\libudis86.a (
	echo Compiling UDis86 Library
	cd src\libudis86
	call compilelibudis86.cmd
	echo UDis86 Compiled!
	cd ../../
)

if not exist lib\libfreetype.a (
	echo Compiling FreeType Library
	cd freetype-2.9
	mingw32-make
	move objs\freetype.a ..\lib\libfreetype.a
	echo FreeType Compiled!
	cd ../
)


SET FLAGS=-s -Wall -O0 -DNDEBUG
SET FLAGS_O3=-s -O3 -Wall -DNDEBUG

call compiledll_common.cmd


pause
exit
