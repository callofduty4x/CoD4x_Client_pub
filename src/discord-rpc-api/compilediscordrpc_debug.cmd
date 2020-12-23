@echo off
@set path=%LOCALAPPDATA%\nasm;%path%
@set path=C:\Program Files (x86)\mingw-w64\i686-5.3.0-win32-dwarf-rt_v4-rev0\mingw32\bin;%path%

echo.
echo Compiling...

g++.exe -std=c++11 -c -g -O0 -Wall "src/connection_win.cpp"
g++.exe -std=c++11 -c -g -O0 -Wall "src/discord_register_win.cpp" -Iinclude
g++.exe -std=c++11 -c -g -O0 -Wall "src/discord_rpc.cpp" -Iinclude
g++.exe -std=c++11 -c -g -O0 -Wall "src/rpc_connection.cpp" -Iinclude
g++.exe -std=c++11 -c -g -O0 -Wall "src/serialization.cpp" -Iinclude

ar cr ../../lib/libdiscord_rpc.a *.o


IF "%AUTOCOMPILE%" == "TRUE" EXIT /b
pause