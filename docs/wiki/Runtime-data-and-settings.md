# Runtime data and settings

## Shipped assets

The executable looks for `assets/` beside itself, not in the process's current working directory.
CMake copies the repository's assets beside each built executable.
Edit files in the source `assets/` tree and rebuild to copy the changes to the build output.

## User settings

At first launch, the renderer creates the user-data directory and copies `assets/config/settings.json` to a writable `settings.json`.
It does not overwrite that file on later launches.

| Platform | Settings path |
| --- | --- |
| Windows | `%LOCALAPPDATA%\OpenGLRenderer\settings.json` |
| Linux | `$XDG_DATA_HOME/openglrenderer/settings.json`, or `~/.local/share/openglrenderer/settings.json` when `XDG_DATA_HOME` is unset |
| macOS | `~/Library/Application Support/OpenGLRenderer/settings.json` |

The JSON object contains a `window` object with these settings:

| Key | Type | Default | Meaning |
| --- | --- | --- | --- |
| `width` | integer | `800` | Initial width; valid range is 1 through 16384 |
| `height` | integer | `600` | Initial height; valid range is 1 through 16384 |
| `title` | string | `OpenGL Renderer` | Window title |
| `wireframe` | boolean | `false` | Draw triangle edges instead of filled polygons |
| `vSync` | boolean | `true` | Synchronize buffer swaps with display refresh |

> [!TIP]
> Missing keys inside a present `window` object keep their defaults and produce a `WARN` entry with the settings path and key.
> A missing `window` section keeps all defaults without warnings.

**Invalid configuration stops startup.** The top level must be a JSON object, and a present `window` value must also be an object.
Malformed JSON, incorrect setting types, and dimensions outside 1 through 16384 produce a configuration error and exit code 2.
A floating-point value is not accepted as a window dimension, even when its numeric value looks close to an integer.

> [!WARNING]
> If the settings file cannot be opened, startup reports an error instead of silently replacing the user's settings with defaults.
> Delete the user settings file to recreate it from the shipped template on the next launch.

On Windows, Debug writes diagnostics to its console.
Release has no console window and redirects stdout and stderr to `%LOCALAPPDATA%\OpenGLRenderer\openglrenderer.log`, replacing the log at each launch.
If that setup fails, the renderer continues when it can and shows fatal startup failures in a Windows error dialog.
Linux and macOS keep their normal terminal output.

## Asset loading

`Paths.cpp` resolves the executable directory and builds asset paths from it.
`main.cpp` loads the vertex and fragment shaders from `assets/shaders/` and the default OBJ mesh from `assets/meshes/cube/`.
The OBJ loader expands indexed triangles into position/normal vertices and uploads them to a vertex array and buffer.

[[Home|Back to wiki home]] · [[Architecture|Read the architecture guide]] · [[Build-and-run|Build and run]]
