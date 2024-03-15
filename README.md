# CoD4x Client
<p align="center">
  <img src="assets/github/banner.png?raw=true" />
</p>

A [CoD4x Server](https://github.com/callofduty4x/CoD4x_Server)-compatible client is a modification of the Call of Duty 4 - Modern Warfare server.

This client version isn't compatible with CoD4x servers that require client authentication, however, it is compatible with servers that have client authentication disabled (`sv_noauth 1`).

Modifications of this client **may** end up in the official release, so merge requests are very welcome.

# Build:

## Build prerequisites:
 - [CMake](https://cmake.org/)
 - [MinGW](http://www.mingw.org/)
 - [NASM](http://www.nasm.us/)

Make sure you've got the necessary binaries in system `$PATH` environment variable. [Click here to learn more.](https://superuser.com/questions/284342/what-are-path-and-other-environment-variables-and-how-can-i-set-or-use-them)

## Building on Windows:
```shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ -G "MinGW Makefiles"
cmake --build build --parallel
```

## Cross-compiling on Linux:
```shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++
cmake --build build --parallel
```

---

After a successful build, the build artifact is located in `build/bin/cod4x_021.dll`.

## License:
The following conditions apply: https://support.activision.com/license

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
