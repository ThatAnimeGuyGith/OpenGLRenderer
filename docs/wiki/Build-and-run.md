# Build and run

The project uses CMake presets and Ninja. It needs a C++20 compiler with `<format>`, which means GCC 13 or newer. CMake stops with an explanatory error if GCC is older.

The `debug` and `release` presets generate separate build trees under `build/debug/` and `build/release/`. The executable's sources are listed explicitly in `CMakeLists.txt`; CMake also adds `src/headers/` as a private include directory. A build copies the repository's `assets/` directory next to the executable so runtime paths do not depend on the current working directory.

## Debug build

```bash
cmake --preset debug
cmake --build --preset debug
```

Run `build/debug/OpenGLRenderer.exe` on Windows or `build/debug/OpenGLRenderer` on Linux. macOS builds in CI but has not been run by hand.

On Windows, Debug uses the console subsystem so developers can see `INFO`, `WARN`, and `ERROR` output. Run from a terminal when you want to inspect it.

## Release package

```bash
cmake --preset release
cmake --build --preset release --target package_release
```

`build/release` is CMake's Release build tree. The distributable folder and ZIP are created separately under `release/<version>_<date>/`. Only files tagged `COMPONENT Runtime` in `CMakeLists.txt` go into the package, so libraries fetched by CMake do not leak into it.

On Windows, Release uses the GUI subsystem and does not open a console window. Its standard output and error streams are redirected at startup to `%LOCALAPPDATA%\OpenGLRenderer\openglrenderer.log`, which keeps warnings and startup errors available without a console. Other platforms are unaffected.
