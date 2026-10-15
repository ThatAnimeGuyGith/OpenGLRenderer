# Runtime data and settings

## Shipped assets

The executable locates `assets/` beside itself, not relative to the process's current working directory. CMake copies the repository's assets beside each built executable. This keeps the shaders, OBJ meshes, and settings template editable and independently packageable. Edit the source `assets/` tree and rebuild to copy changes to the build output.

## User settings

At first launch, the renderer creates the user-data directory and copies `assets/config/settings.json` to a writable `settings.json`. It does not overwrite that file on later launches.

| Platform | Settings path |
| --- | --- |
| Windows | `%LOCALAPPDATA%\OpenGLRenderer\settings.json` |
| Linux | `$XDG_DATA_HOME/openglrenderer/settings.json`, or `~/.local/share/openglrenderer/settings.json` when `XDG_DATA_HOME` is unset |

The JSON object contains a `window` object with these settings:

| Key | Type | Default | Meaning |
| --- | --- | --- | --- |
| `width` | integer | `800` | Initial window width; valid range is 1 through 16384 |
| `height` | integer | `600` | Initial window height; valid range is 1 through 16384 |
| `title` | string | `OpenGL Renderer` | Window title |
| `wireframe` | boolean | `false` | Draw triangle edges instead of filled polygons |
| `vSync` | boolean | `true` | Synchronize buffer swaps with the display refresh |

Missing keys keep their defaults and each missing key produces a `WARN` entry containing the settings path and key. Invalid types and out-of-range dimensions are configuration errors; the application logs the error and exits with code 2. A missing `window` object also leaves all defaults in place, while a `window` value of the wrong JSON type is an error.

Delete the user settings file to recreate it from the shipped template. On Windows, Debug writes diagnostics to its console. Release has no console window and redirects stdout and stderr to `%LOCALAPPDATA%\OpenGLRenderer\openglrenderer.log`; the log file is replaced at each launch. Linux and macOS keep their normal terminal output.

## Asset loading

`Paths.cpp` resolves the executable directory and builds asset paths from it. `main.cpp` loads the vertex and fragment shaders from `assets/shaders/` and the default OBJ mesh from `assets/meshes/cube/`. The OBJ loader expands indexed triangles into position/normal vertices and uploads them to a vertex array and buffer. See [[Architecture]] for the full startup and rendering flow.
