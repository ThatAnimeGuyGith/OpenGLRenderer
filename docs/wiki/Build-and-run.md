# Build and run

The project uses CMake presets and Ninja.

## Debug build

```bash
cmake --preset debug
cmake --build --preset debug
```

Run `build/debug/OpenGLRenderer.exe` on Windows or `build/debug/OpenGLRenderer` on Linux and macOS.

## Release package

```bash
cmake --preset release
cmake --build --preset release --target package_release
```

`build/release` is CMake's Release build tree. The distributable folder and ZIP are created separately under `release/<version>_<date>/`.
