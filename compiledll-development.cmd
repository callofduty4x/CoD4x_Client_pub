@echo off

cls
color 40
echo -------------------------------------------------------------
echo -                          WARNING                          -
echo -         This build script is for DEVELOPMENT ONLY!        -
echo -   DO NOT GIVE THE COMPILED DLL TO UNAUTHORIZED PERSONS!   -
echo -     If you meant to run the release script, exit now!     -
echo -------------------------------------------------------------
pause
color 07
cls

SET "AUTOCOMPILE=TRUE"

if not exist lib\libsteam_integration.a (
	echo Compiling Steam API
	cd src/steam_api
	call compilesteamapi_dbg.cmd
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
	call compilediscordrpc_debug.cmd
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


SET FLAGS=-g -Wall -O0 -D COD4XDEV
SET FLAGS_O3=-g -Wall -O0 -D COD4XDEV

call compiledll_common.cmd

pause

