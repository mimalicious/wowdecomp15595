# wowdecomp15595

A C++ decompilation of the World of Warcraft 4.3.4 client (build 15595, Cataclysm).

Every function is reconstructed from the original binaries and keeps the address it came from in an
`// OFFSET:` tag, so each one can be compared against the original. The source tree follows the layout
of the original client (`Engine/Source/<Module>`, `WoW/Source/<Module>`, original file names).

Two original binaries are used as references:

| Binary | Architecture |
|---|---|
| `Wow.exe` 4.3.4.15595 | x86 (32-bit) |
| `Wow-64.exe` 4.3.4.15595 | x64 (64-bit) |

x64 is the main build target. Win32 still builds and is used to check struct layouts against the 32-bit
original.

## Progress

The numbers below measure how much of the original binary has a counterpart in this source tree. They do
**not** say how much of the game works: the client builds into a complete executable that starts and runs. A
ported function can still behave differently from the original.

Measured on 2026-10-16. A function counts as ported when its address is tagged in the source.

| Binary | Ported | Total | Share |
|---|---:|---:|---:|
| x86 functions | 30,790 | 31,300 | 98.4% |
| x86 code bytes | 6,531,012 | 6,653,634 | 98.2% |
| x64 functions | 26,401 | 38,179 | 69.2% |
| x64 code bytes | 7,553,054 | 9,480,010 | 79.7% |

The x86 total leaves out 9,196 functions that are compiler runtime code, STL template instances or linked
third-party libraries (FMOD Ex, FreeType, zlib, PCRE, ...). Those come from the compiler or from upstream sources
instead of being decompiled. The x64 total includes them, and many x64 addresses are still being added to
functions that are already ported, so the x64 share is lower than the code it covers.

### Address tags

```cpp
// OFFSET: 0x00408430/0x14000A7B0
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
```

| Tag | Meaning |
|---|---|
| `0x<x86>/0x<x64>` | 15595 addresses in `Wow.exe` / `Wow-64.exe`; the x64 half is left out while unknown |
| `[0x...]` | One body shared by several addresses (identical functions folded by the linker) |
| `%0x...` | Address in 4.0.6a (13623): carried over from the previous build, not reworked for 4.3.4 yet |
| `#0x...` | Address in 3.3.5a (12340): carried over from the previous build, not reworked for 4.3.4 yet |
| `// INLINED` | The function exists in the source but the compiler inlined it everywhere in 15595 |

## Requirements

- Windows 10 or 11
- Visual Studio 2022 with the **Desktop development with C++** workload (MSVC v143 and a Windows SDK)
- CMake 3.15 or newer (the one that ships with Visual Studio works)
- For Win32 builds only: [NASM](https://www.nasm.us/), on `PATH` or passed as `-DNASM_EXECUTABLE=<path>`, and
  `DivxDecoder.dll` copied to `ThirdParty/DivxDecoder/` (the build copies it next to the executables; the x64
  build uses its own decoder, `XvidDecoder.dll`, instead)
- A World of Warcraft **4.3.4 (15595)** client. No game data is included in this repository.

## Building

From a Developer PowerShell or any shell with `cmake` on `PATH`:

```powershell
cd wowdecomp15595

# x64 (main target)
cmake -S . -B build/x64 -G "Visual Studio 17 2022" -A x64
cmake --build build/x64 --config Release --target WowClient Wow --parallel

# Win32 (optional, needs NASM)
cmake -S . -B build/win32 -G "Visual Studio 17 2022" -A Win32
cmake --build build/win32 --config Release --target WowClient Wow --parallel
```

In-source builds are refused, so keep the build directory separate (`build/` above). Ninja works as well
(`-G Ninja` from a Developer PowerShell).

The executables are written to `bin/Win/`:

| Target | x64 | Win32 |
|---|---|---|
| `WowClient` | `WowClient64.exe` | `WowClient32.exe` |
| `Wow` | `Wow64.exe` | `Wow32.exe` |
| `XvidDecoder` | `XvidDecoder.dll` | (not built) |

`WowClient` is the client with a crash logger: it opens a console window and writes `WowClient_crash.log` when
the client crashes. `Wow` is the same client without it.

Debug output (`DebugLog`) is compiled out of Release builds.

### Options

| CMake option | Default | Effect |
|---|---|---|
| `WOWDECOMP_DISABLE_SIGNATURE_CHECKS` | `OFF` | Skip the MPQ file and `.toc.sig` signature checks, so modified game data and interface files load |
| `WOWCLIENT_OUTPUT_OVERRIDE` | empty | Name the `WowClient` executable something other than `WowClient64` / `WowClient32` |

## Running

1. Copy the `Data` folder of your 4.3.4 client into `bin/Win/`, so you have `bin/Win/Data/`.
2. Point the client at your server: `SET realmlist "<address>"` in `bin/Win/WTF/Config.wtf`.
3. Start `bin/Win/WowClient64.exe` (or `Wow64.exe`). Add `-windowed` to start in a window.

## Layout

| Path | Contents |
|---|---|
| `Engine/Source/` | Engine modules: Storm, Gx (Direct3D 9 / OpenGL), Model2, Frame/FrameXML UI, SoundEngine, Lua, ... |
| `WoW/Source/` | Game modules: objects, world rendering, UI, networking (including the JAM message layer), database, glue screens |
| `WoW/Common/` | Code shared by the game modules |
| `ThirdParty/` | Libraries linked from their upstream sources |
| `Tools/WowClient/` | The `WowClient` executable target |
| `Tools/XvidDecoder/` | x64 movie decoder that replaces the original 32-bit `DivxDecoder.dll` |

## Special thanks

- [binana](https://github.com/thunderbrewhq/binana)
- [thunderbrew](https://github.com/thunderbrewhq/thunderbrew)
- [whoa](https://github.com/whoahq/whoa)
- TOMRUS, for the initial IDB
- Everyone who has reported bugs so far
- The WoW modding community
