@echo off

echo.
echo Compiling...

g++.exe -c -s -O0 -Wall "steam_api.cpp"
g++.exe -c -s -O0 -Wall "steam_interface.cpp"


ar cr ..\..\lib\libsteam_integration.a *.o

IF "%AUTOCOMPILE%" == "TRUE" EXIT /b
pause