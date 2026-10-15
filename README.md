# OpenGL Renderer

A small OpenGL 3.3 renderer written in C++20.
I am learning as I write and the long-term plan is making it into a renderer for a game.

Right now, it loads lights and a small cube OBJ mesh.

**Status:** Early development (v0.2.0).

> [!WARNING]
> The code changes often and nothing here is stable yet.

## Building

You need a C++20 compiler, CMake 3.24 or newer, Ninja, and git.

CMake downloads GLFW, nlohmann/json, and tinyobjloader the first time you configure.

**Actively tested platforms:**
- Windows (MSYS2 / MinGW-w64)
- Linux

You can find instructions to setting up the project yourself *bellow*:


### Windows (MSYS2, MinGW-w64)

1. Install [MSYS2](https://www.msys2.org/).
2. In an MSYS2 UCRT64 terminal, install the tools:
   ```
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja git
   ```
3. Add `C:\msys64\ucrt64\bin` to your `PATH` so `g++` works in PowerShell and VS Code.
4. Build and run:
   ```
   cmake --preset debug
   cmake --build --preset debug
   .\build\debug\OpenGLRenderer.exe
   ```

### Linux

Arch and derivatives (CachyOS, Manjaro, etc.):

```
sudo pacman -S base-devel cmake ninja git glfw nlohmann-json
```

Debian and Ubuntu:

```
sudo apt install build-essential cmake ninja-build git pkg-config \
    libwayland-dev libxkbcommon-dev wayland-protocols \
    libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

Then:

```
cmake --preset debug
cmake --build --preset debug
./build/debug/OpenGLRenderer
```

> [!NOTE]
> On Linux, CMake uses your system's GLFW and nlohmann/json when they are installed.
> If they are not, it downloads and builds them, which needs the X11/Wayland development packages listed above.

### Presets

|  Preset   |                      Build Config                     | Output directory |
| --------- | ----------------------------------------------------- | ---------------- |
| `debug`   | Debug, with compiler optimizations typically disabled | `build/debug/`   |
| `release` | Optimized Release build                               | `build/release/` |

Use either name with `cmake --preset <name>` and `cmake --build --preset <name>`.

### VS Code

Install the recommended C/C++ and CMake Tools extensions when VS Code prompts you. Opening the repository configures the Debug preset and gives IntelliSense the generated compile commands.

Use `Ctrl+Shift+B` to configure and build the Debug target, then run the `OpenGL Renderer: run Debug` task from `Terminal: Run Task`.

### Making a release

```
cmake --preset release
cmake --build --preset release --target package_release
```

This creates `release/<version>_<date>/` with a ready-to-run program folder and a `.zip` of it. The version number comes from `project(... VERSION ...)` in `CMakeLists.txt`. Change it there.

## Running

The program reads `assets/` next to the executable, so you can start it from any working directory.
Every build copies `assets/` beside the executable.

To test shader or configuration changes without recompiling C++, edit the copied assets next to the executable.
Changes made in the repository's source `assets/` directory are copied on the next build.

Window size, title, and wireframe mode are set in `assets/config/settings.json`.

> [!TIP]
> Press `Esc` to quit.

## Project layout

|     Path     |                             Use                             |
|--------------|-------------------------------------------------------------|
| src/         |  C++ sources and headers (.hpp beside .cpp)                 |
| assets/      | Shaders and config, placed next to the executable on build  |
| third_party/ | glad, the OpenGL loader (generated code, kept in the repo)  |
| cmake/       | Helper scripts used by CMake                                |
| build/       | Build output (ignored by git)                               |
| release/     | Packaged releases (ignored by git)                          |

## License

This project uses the [PolyForm Noncommercial License 1.0.0](LICENSE). In plain terms:

- You can read the code, learn from it, and run it for noncommercial purposes.
- You can fork it and make changes for noncommercial purposes.
- You **cannot** sell it or use it commercially.

If you share it, you must include the license text and the `Required Notice:` line from `LICENSE`. Please link to this repository instead of re-uploading builds to other sites, so people always get the current version.

The `LICENSE` file is the only legally binding text. This summary is a convenience and does not change it.

> [!IMPORTANT]
> GitHub still lets people fork a public repo under GitHub's own terms.
> Noncommercial redistribution is permitted subject to the license's conditions, so please link to this repository instead of rehosting builds, so users can find the latest version.

## Contributing

Issues and pull requests are welcome.
By submitting a contribution, you confirm that you wrote it *(or have the right to submit it)* and that you license it under the same terms as the project, and also grant the project owner the right to use it in commercial releases of this project or of games built from it.

**If you are not comfortable with that, please open an issue to discuss the idea instead.**

## Third-party software

| Library | Purpose | License |
| ------- | ------- | ------- |
| [GLFW](https://www.glfw.org/) 3.5.1 | Window, input, OpenGL context | zlib |
| [glad](https://github.com/Dav1dde/glad) 0.1.36 | OpenGL function loader | MIT / public domain |
| [nlohmann/json](https://github.com/nlohmann/json) 3.12.0 | JSON parsing | MIT |
| [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) 2.0.0rc13 | Wavefront OBJ loading | MIT |

The cube demo mesh was created by Morgan McGuire and released into the public domain. Its source is noted in `assets/meshes/cube/cube.obj`.
