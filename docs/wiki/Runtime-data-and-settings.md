# Runtime data and settings

The executable reads shipped assets from an `assets/` directory beside itself. Shaders, meshes, textures, and other large content stay external to the executable so they can be updated or packaged independently.

At first launch, the renderer copies `assets/config/settings.json` to a writable user settings file. Later launches read that user file without overwriting it.

| Platform | Settings path |
| --- | --- |
| Windows | `%LOCALAPPDATA%\OpenGLRenderer\settings.json` |
| Linux | `$XDG_DATA_HOME/openglrenderer/settings.json`, or `~/.local/share/openglrenderer/settings.json` |

Delete the user settings file to restore the shipped defaults.
