@echo off
@set path=%LOCALAPPDATA%\nasm;%path%

echo.
echo Compiling...

gcc.exe -c -s -O0 -Wall "decode.c"
gcc.exe -c -s -O0 -Wall "itab.c"
gcc.exe -c -s -O0 -std=gnu99 -Wall "syn.c"
gcc.exe -c -s -O0 -Wall "syn-intel.c"
gcc.exe -c -s -O0 -Wall "udis86.c"

ar cr ..\..\lib\libudis86.a *.o
del *.o


IF "%AUTOCOMPILE%" == "TRUE" EXIT /b
pause