# OpenGL Renderer

A small OpenGL 3.3 renderer written in C++20. I am learning with it. The long-term plan is a game. Right now it opens a window and draws one orange triangle.

**Status.** Early development (v0.1.0). The code changes often and nothing here is stable yet.

## Building

You need a C++20 compiler, CMake 3.24 or newer, Ninja, and git. CMake downloads GLFW and nlohmann/json the first time you configure.

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

On Linux, CMake uses your system's GLFW and nlohmann/json when they are installed. If they are not, it downloads and builds them, which needs the X11/Wayland development packages listed above.

### Presets

| Preset    | What it is                                    | Output directory |
| --------- | --------------------------------------------- | ---------------- |
| `debug`   | No optimization, debug info. Use for development. | `build/debug/`   |
| `release` | Optimized build. Use for releases.            | `build/release/` |

Use either name with `cmake --preset <name>` and `cmake --build --preset <name>`.

### Making a release

```
cmake --preset release
cmake --build --preset release --target package_release
```

This creates `release/<version>_<date>/` with a ready-to-run program folder and a `.zip` of it. The version number comes from `project(... VERSION ...)` in `CMakeLists.txt`. Change it there.

## Running

The program reads `assets/` next to the executable, so you can start it from any working directory. The build copies (Windows) or links (Linux) that folder beside the exe. Press `Esc` to quit.

Window size, title, and wireframe mode are set in `assets/config/settings.json`.

## Project layout

```
src/            C++ sources and headers (.hpp beside .cpp)
assets/         Shaders and config, placed next to the executable on build
third_party/    glad, the OpenGL loader (generated code, kept in the repo)
cmake/          Helper scripts used by CMake
build/          Build output (ignored by git)
release/        Packaged releases (ignored by git)
```

## License

This project uses the [PolyForm Noncommercial License 1.0.0](LICENSE). In plain terms:

- You can read the code, learn from it, and run it for noncommercial purposes.
- You can fork it and make changes for noncommercial purposes.
- You **cannot** sell it or use it commercially.

If you share it, you must include the license text and the `Required Notice:` line from `LICENSE`. Please link to this repository instead of re-uploading builds to other sites, so people always get the current version.

The `LICENSE` file is the only legally binding text. This summary is a convenience and does not change it.

GitHub still lets people fork a public repo under GitHub's own terms. PolyForm Noncommercial also allows noncommercial redistribution, so it does not stop someone from posting a copy elsewhere. A polite request is all we have for that part.

## Contributing

Issues and pull requests are welcome. By submitting a contribution, you confirm that you wrote it (or have the right to submit it) and that you license it under the same terms as the project, and also grant the project owner the right to use it in commercial releases of this project or of games built from it. If you are not comfortable with that, please open an issue to discuss the idea instead.

## Third-party software

| Library | Purpose | License |
| ------- | ------- | ------- |
| [GLFW](https://www.glfw.org/) 3.5.1 | Window, input, OpenGL context | zlib |
| [glad](https://github.com/Dav1dde/glad) 0.1.36 | OpenGL function loader | MIT / public domain |
| [nlohmann/json](https://github.com/nlohmann/json) 3.12.0 | JSON parsing | MIT |
