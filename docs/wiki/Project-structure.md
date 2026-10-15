# Project structure

| Path | Purpose |
| --- | --- |
| `src/main.cpp` | Application startup, render-loop coordination, and shutdown |
| `src/*.cpp` | Implementations for settings, paths, logging, window/context, shaders, mesh loading, and errors |
| `src/headers/*.hpp` | Project-owned C++ interfaces used by the implementation files |
| `assets/config/` | Shipped template for per-user settings |
| `assets/meshes/` | Shipped Wavefront OBJ meshes |
| `assets/shaders/` | GLSL vertex and fragment shaders |
| `third_party/glad/` | Committed GLAD loader source and headers |
| `cmake/` | Release packaging helper scripts |
| `docs/wiki/` | Source Markdown published to the GitHub wiki |
| `.github/workflows/` | Build and wiki-publishing workflows |
| `build/` | Generated CMake build trees and executables; not source files |
| `release/` | Generated release folders and archives |

## Source responsibilities

- `main.cpp` coordinates the application lifetime: read settings, create the OpenGL context, construct the shader and mesh, render frames, and release GL resources before the context is destroyed.
- `headers/Settings.hpp` defines settings and the loader interface; `Settings.cpp` parses JSON and validates the values used by the window.
- `headers/Paths.hpp` and `Paths.cpp` locate the executable, shipped assets, and writable user-data directory.
- `headers/Window.hpp` and `Window.cpp` own GLFW initialization, window creation, GLAD initialization, callbacks, and keyboard input.
- `headers/Shader.hpp` and `Shader.cpp` read GLSL files, compile shaders, link the program, and set matrix uniforms.
- `headers/ObjMesh.hpp` and `ObjMesh.cpp` load OBJ geometry and upload vertex data to OpenGL buffers.
- `headers/Log.hpp` and `Log.cpp` provide the shared diagnostic output path; `headers/Error.hpp` and `Error.cpp` define categorized renderer errors and exit codes.

The renderer target's private include directory is `src/headers`, so application files use includes such as `#include "Settings.hpp"`. `CMakeLists.txt` lists the implementation and header files explicitly. When adding a new implementation, add its `.cpp` and header to the target list; put project-owned headers in `src/headers/`.

The GitHub Actions build workflow configures and compiles both Debug and Release presets on Ubuntu, macOS, and Windows. It uploads only the Release executable and assets as a short-lived workflow artifact for each platform.
