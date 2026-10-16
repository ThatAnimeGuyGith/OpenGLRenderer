# Build and run

The project uses CMake presets with Ninja.
You need a C++20 compiler with `<format>` support, CMake 3.24 or newer, Ninja, and Git.
GCC must be version 13 or newer; CMake reports a clear error if it detects an older version.

> [!IMPORTANT]
> Running the renderer requires a desktop session and graphics driver that supports OpenGL 3.3.
> The settings tests do not create a window or require a GPU.

The executable's sources are listed explicitly in `CMakeLists.txt`.
A build copies the repository's `assets/` directory next to the executable so runtime paths do not depend on the current working directory.

## Debug build

```bash
cmake --preset debug
cmake --build --preset debug
```

The executable is written to `build/debug/` beside a copy of `assets/`.
Run `build/debug/OpenGLRenderer.exe` on Windows or `build/debug/OpenGLRenderer` on Linux.
macOS builds are covered by CI, but have not been tested interactively.

On Windows, Debug uses the console subsystem so developers can see `INFO`, `WARN`, and `ERROR` output.
Run from a terminal to inspect these messages.

## Run tests

Build the project, then run CTest for the configuration you built:

```bash
ctest --test-dir build/debug --output-on-failure -C Debug
ctest --test-dir build/release --output-on-failure -C Release
```

The current suite tests JSON settings and validation.
It does not initialize GLFW or OpenGL.

## Release package

```bash
cmake --preset release
cmake --build --preset release --target package_release
```

The build tree is `build/release/`.
The ready-to-run folder and platform ZIP are written to `release/<version>_<date>/`.
The packaging target checks that required files are present and that build or test files are not included.

On Windows, Release uses the GUI subsystem and does not open a console window.
At startup, it redirects standard output and error to `%LOCALAPPDATA%\OpenGLRenderer\openglrenderer.log`.
Linux and macOS keep their normal terminal output.

## Continuous integration

See the [Build workflow](https://github.com/ThatAnimeGuyGith/OpenGLRenderer/blob/main/.github/workflows/build.yml).
It configures, builds, and tests Debug and Release on Ubuntu, macOS, and Windows.
Each Release job also creates and verifies its package.
On pushes to `main` and manual workflow runs, CI uploads the executable/assets and package as separate artifacts for each operating system.

[[Home|Back to wiki home]] · [[Runtime-data-and-settings|Runtime data and settings]] · [[Architecture|Read the architecture guide]]
