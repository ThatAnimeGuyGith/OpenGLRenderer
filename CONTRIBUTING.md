# Contributing

Issues and pull requests are welcome.

By submitting a contribution, you confirm that you wrote it or have the right to submit it.
You agree to license the contribution under the same terms as the project and grant the project owner the right to use it in commercial releases of this project or of games built from it.

**If you are not comfortable with these terms, please open an issue to discuss them before submitting a contribution.**

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