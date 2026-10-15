# Architecture

The renderer is a small single-window application. `main.cpp` owns the high-level sequence; the other modules provide focused services for settings, paths, the OpenGL window, shaders, mesh data, diagnostics, and errors. Project interfaces live in `src/headers/`, while implementations live in `src/`.

## Startup sequence

1. On Windows Release, `main()` attempts to redirect stdout and stderr to `openglrenderer.log` in the user-data directory. Debug and non-Windows builds keep their ordinary output destinations.
2. `userSettingsPath()` creates the platform-specific user-data directory and copies the shipped settings template on first launch. `loadSettings()` parses the JSON and applies defaults for missing values.
3. `GlfwSession` installs the GLFW error callback, initializes GLFW, and requests an OpenGL 3.3 core context. It terminates GLFW when its lifetime ends.
4. `createOpenGLWindow()` creates the window, makes its context current, applies VSync, initializes GLAD, and configures the framebuffer callback and viewport.
5. `createShaderProgram()` reads both GLSL files, compiles each shader, links them into a program, then releases the temporary shader objects.
6. `ObjMesh` loads the OBJ file with tinyobjloader, prepares triangle vertices and normals, and uploads them to a VAO/VBO. The initial render state keeps references to the shader program and mesh.
7. The application enters the GLFW event/render loop. Initialization failures throw categorized `RendererError` exceptions and are logged before returning their category-specific exit code.

## Frame rendering

`drawFrame()` queries the framebuffer dimensions and skips rendering when either dimension is zero, such as while minimized. It clears the color and depth buffers, builds model, view, and perspective projection matrices, sends those matrices to the shader, draws the mesh, and swaps the front and back buffers.

The model matrix rotates the object around its vertical axis using GLFW's elapsed time. The view matrix moves the camera back along the Z axis. The projection matrix uses the current framebuffer aspect ratio. The vertex shader transforms positions and normals; the fragment shader applies a fixed directional diffuse light with a small ambient term and a warm base color. Lighting is currently shader-defined, not a user setting.

GLFW polls events after each frame. Pressing `Esc` marks the window to close. A window-refresh callback also draws a frame when the operating system requests a redraw.

## Mesh preparation

`ObjMesh` asks tinyobjloader to triangulate the input. The loader centers the mesh on its bounds and scales its largest dimension to 2 units. For every triangle it emits three vertices with positions and normals: valid normals from the OBJ are used, otherwise a face normal is calculated. OpenGL receives interleaved position/normal data, with position at attribute location 0 and normal at location 1. Rendering uses `glDrawArrays(GL_TRIANGLES, ...)`.

## Resource lifetime

The OpenGL context must remain current while GL resources are deleted. `main()` clears the GLFW callbacks and user pointer and deletes the shader program before returning. C++ then destroys the mesh, which deletes its buffers and vertex array, and finally destroys `GlfwSession`, which terminates GLFW and closes the context/window. This declaration/destruction order is intentional.

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

`main.cpp` composes these interfaces. `CMakeLists.txt` explicitly lists the implementation and header files, adds `src/headers/` to the renderer target's include path, and links GLAD, GLFW, nlohmann/json, and tinyobjloader. GLAD is committed under `third_party/`; CMake fetches the other dependencies when needed.

## Extending the project

Put a project-owned declaration in `src/headers/` and its implementation in `src/`. Add both paths to the `OpenGLRenderer` source list in `CMakeLists.txt`; the target include path already makes the header available with a simple quoted include. Keep runtime content in `assets/` and resolve it through `assetPath()` so the application remains independent of its working directory. Update the relevant wiki page when the runtime flow or file responsibilities change.