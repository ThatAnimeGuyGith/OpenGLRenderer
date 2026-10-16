# Architecture

The renderer is a small single-window application.
[`main.cpp`](https://github.com/ThatAnimeGuyGith/OpenGLRenderer/blob/main/src/main.cpp) owns the high-level sequence.

Other modules handle settings, paths, the OpenGL window, shaders, mesh data, diagnostics, and errors.
Project headers live in `src/headers/`, with implementations in `src/`.

<img src="https://raw.githubusercontent.com/ThatAnimeGuyGith/OpenGLRenderer/main/.github/docs/svg/architecture.svg" alt="OpenGLRenderer startup, render loop, and shutdown flow" width="750">

## Startup sequence

1. On Windows Release, `main()` tries to redirect standard output and error to `openglrenderer.log` in the user-data directory.
	Debug and non-Windows builds keep their usual output destinations.

2. `userSettingsPath()` creates the user-data directory and copies the shipped settings template on first launch.
	`loadSettings()` parses the JSON and applies defaults for missing values.

3. `GlfwSession` installs the GLFW error callback, initializes GLFW, and requests an OpenGL 3.3 core context.
	Its destructor terminates GLFW when the session ends.

4. `createOpenGLWindow()` creates a window and makes its context current.
	It applies VSync, initializes GLAD, and configures the framebuffer callback and viewport.

5. `createShaderProgram()` reads both GLSL files, compiles the shaders, links the program, and releases the temporary shader objects.

6. `ObjMesh` loads the OBJ file with tinyobjloader, prepares triangle vertices and normals, and uploads them to a VAO and VBO.
	The render state refers to the shader program and mesh.

7. The application enters the GLFW event and render loop.
	Initialization failures throw categorized `RendererError` exceptions, which are logged before the application returns an error code.

## Frame rendering

`drawFrame()` queries the framebuffer dimensions and skips rendering if either dimension is zero, such as while minimized.

It clears the color and depth buffers, then builds the model, view, and perspective projection matrices.
It sets the shader uniforms, draws the mesh, and swaps buffers.

The model matrix rotates the object around its vertical axis using GLFW's elapsed time.
The view matrix moves the camera back along the Z axis.
The projection matrix uses the current framebuffer aspect ratio.

The vertex shader transforms positions and normals.
The fragment shader applies fixed directional diffuse lighting, a small ambient term, and a warm base color.
Lighting is defined in the shader, not in the settings file.

GLFW polls events after each frame.
Pressing `Esc` marks the window to close.
A window-refresh callback draws a frame when the operating system requests a redraw.

## Mesh preparation

`ObjMesh` asks tinyobjloader to triangulate the input.
It centers the mesh on its bounds and scales its largest dimension to 2 units.

For each triangle, it emits three vertices with positions and normals.
Valid OBJ normals are used; otherwise, the loader calculates a face normal.

OpenGL receives interleaved position/normal data, with position at attribute location 0 and normal at location 1.
Rendering uses `glDrawArrays(GL_TRIANGLES, ...)`.

## Resource lifetime

The OpenGL context must remain current while GL resources are deleted.
`main()` clears the GLFW callbacks and user pointer, then deletes the shader program before returning.

C++ destroys the mesh next, which deletes its buffers and vertex array.
Finally, `GlfwSession` terminates GLFW and closes the window and context.

> [!IMPORTANT]
> This declaration and destruction order is intentional.
> Destroying the context before the mesh would prevent the mesh destructor from releasing its OpenGL objects correctly.

## Module map

| Interface | Implementation | Responsibility |
| --- | --- | --- |
| `headers/Settings.hpp` | `Settings.cpp` | JSON settings, defaults, and validation |
| `headers/Paths.hpp` | `Paths.cpp` | Executable, asset, and user-data paths |
| `headers/Window.hpp` | `Window.cpp` | GLFW lifecycle, window/context creation, callbacks, and input |
| `headers/Shader.hpp` | `Shader.cpp` | GLSL file loading, compilation, program linking, and uniforms |
| `headers/ObjMesh.hpp` | `ObjMesh.cpp` | OBJ parsing and OpenGL mesh buffers |
| `headers/Log.hpp` | `Log.cpp` | Shared INFO/WARN/ERROR diagnostic output |
| `headers/Error.hpp` | `Error.cpp` | Categorized exceptions and process exit codes |

`main.cpp` composes these interfaces.
`CMakeLists.txt` lists application source and header files explicitly and links GLAD, GLFW, nlohmann/json, and tinyobjloader.

GLAD is committed under `third_party/`.
CMake uses system-installed GLFW and nlohmann/json on Linux when available, fetches them when needed, and always fetches tinyobjloader.

## Extending the project

Put project-owned declarations in `src/headers/` and their implementations in `src/`.
Add both files to the `OpenGLRenderer` source list in `CMakeLists.txt`.

Keep runtime content in `assets/` and resolve it through `assetPath()` so the application does not depend on its current working directory.
Update the relevant wiki page when runtime behavior or file responsibilities change.

[[Home|Back to wiki home]] · [[Project-structure|View the project structure]] · [[Build-and-run|Build and run]]