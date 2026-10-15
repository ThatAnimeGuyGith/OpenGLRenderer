# Build and run

The project uses CMake presets and Ninja. It needs a C++20 compiler with `<format>`, which means GCC 13 or newer. CMake stops with an explanatory error if GCC is older.

## Debug build

```bash
cmake --preset debug
cmake --build --preset debug
```

Run `build/debug/OpenGLRenderer.exe` on Windows or `build/debug/OpenGLRenderer` on Linux. macOS builds in CI but has not been run by hand.

## Release package

```bash
cmake --preset release
cmake --build --preset release --target package_release
```

`build/release` is CMake's Release build tree. The distributable folder and ZIP are created separately under `release/<version>_<date>/`. Only files tagged `COMPONENT Runtime` in `CMakeLists.txt` go into the package, so libraries fetched by CMake do not leak into it.
