# Contributing

Before submitting a change, read the contribution terms in the [README](README.md#contributing). They describe the rights associated with contributions to this project.

## Tools

Use a C++20 compiler with `<format>` support, CMake 3.24 or newer, Ninja, and Git. CMake uses system-installed GLFW and nlohmann/json on Linux when available, and fetches either when missing. It always fetches tinyobjloader. Windows builds fetch GLFW and nlohmann/json as well. Building GLFW on Linux also requires the relevant X11 or Wayland development packages; the [README](README.md#building) lists the current packages.

## Build and test

From the repository root, configure and build Debug, then run its tests:

```bash
cmake --preset debug
cmake --build --preset debug
ctest --test-dir build/debug --output-on-failure
```

Run the same checks with `release` in place of `debug` when changing Release behavior. To build the distributable package:

```bash
cmake --preset release
cmake --build --preset release --target package_release
```

The package is written under `release/<version>_<date>/`.

## Changes

Keep changes focused and easy to understand. Add or update tests for behavior changes, and avoid adding dependencies for test-only work when the standard library is enough. Before submitting, make sure the relevant build and CTest commands pass; build the release package when changing packaging or release behavior.