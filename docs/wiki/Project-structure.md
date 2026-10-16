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
| `tests/` | Context-free settings tests |
| `docs/wiki/` | Source Markdown published to the GitHub wiki |
| `.github/workflows/` | Build and wiki-publishing workflows |
| `build/` | Generated CMake build trees and executables; not source files |
| `release/` | Generated release folders and archives |

## Source responsibilities

- **Application entry point:** [`src/main.cpp`](https://github.com/ThatAnimeGuyGith/OpenGLRenderer/blob/main/src/main.cpp).
	It reads settings, creates the OpenGL context, and constructs the shader and mesh.
	It runs the render loop and releases GL resources before context shutdown.
- **Settings:** `src/headers/Settings.hpp` defines the settings and loader interface; `src/Settings.cpp` parses JSON and validates window values.
- **Paths:** `src/headers/Paths.hpp` and `src/Paths.cpp` locate the executable, shipped assets, and writable user-data directory.
- **Window:** `src/headers/Window.hpp` and `src/Window.cpp` manage GLFW initialization and window/context creation.
	They also initialize GLAD and handle callbacks and keyboard input.
- **Shaders:** `src/headers/Shader.hpp` and `src/Shader.cpp` load GLSL files, compile shaders, link the program, and set uniforms.
- **Mesh:** `src/headers/ObjMesh.hpp` and `src/ObjMesh.cpp` load OBJ geometry and upload vertex data to OpenGL buffers.
- **Diagnostics:** `src/headers/Log.hpp` and `src/Log.cpp` provide shared output.
	`src/headers/Error.hpp` and `src/Error.cpp` define categorized errors and exit codes.

The renderer target uses `src/headers/` as a private include directory.
Application files can include a project header with a short include such as `#include "Settings.hpp"`.
When adding an implementation, list its `.cpp` and header in `CMakeLists.txt`.

## Build and release files

The `debug` and `release` CMake presets write to `build/debug/` and `build/release/`.
The `package_release` target writes a ready-to-run folder and platform ZIP under `release/<version>_<date>/`.
See the [Build workflow](https://github.com/ThatAnimeGuyGith/OpenGLRenderer/blob/main/.github/workflows/build.yml).
It runs tests in both configurations on Ubuntu, macOS, and Windows.
Release jobs also create and verify a package.
On pushes to `main` and manual runs, CI uploads the executable/assets and package as separate artifacts for each operating system.

[[Home|Back to wiki home]] · [[Architecture|Read the architecture guide]] · [[Build-and-run|Build and run]]
