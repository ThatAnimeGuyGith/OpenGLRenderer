cmake_minimum_required(VERSION 3.24)

# The version lives here and nowhere else. The release script reads it.
project(OpenGLRenderer VERSION 0.3.2 LANGUAGES C CXX)

# <format> arrived in GCC 13. Fail here, with a clear
# message, instead of deep inside the compiler with "format: No such file".
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 13)
    message(FATAL_ERROR
        "GCC ${CMAKE_CXX_COMPILER_VERSION} is too old. OpenGLRenderer uses <format>, "
        "which needs GCC 13 or newer. Ubuntu 22.04 and Debian 12 ship older compilers by default.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # editors (clangd, VS Code) read this

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
include(FetchContent)

# On Windows, always download and build our own copies so the release build
# never depends on whatever happens to be installed in MSYS2.
# On Linux, use the system's GLFW/nlohmann-json when installed (pacman -S glfw
# nlohmann-json) and only download if they are missing.
if(WIN32)
    set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE NEVER)
endif()

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
set(JSON_BuildTests     OFF CACHE BOOL "" FORCE)
set(JSON_Install        OFF CACHE BOOL "" FORCE)

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.5.1
    GIT_SHALLOW    TRUE
    FIND_PACKAGE_ARGS 3.4 NAMES glfw3
)
FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.12.0
    GIT_SHALLOW    TRUE
    FIND_PACKAGE_ARGS 3.11
)
FetchContent_Declare(tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG        v2.0.0rc13
    GIT_SHALLOW    TRUE
)
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.93.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(glfw nlohmann_json tinyobjloader imgui)

include(CTest)

# glad is generated code, so it is committed in third_party/glad.
add_library(glad STATIC third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC third_party/glad/include)
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})   # glad uses dlopen on Linux

add_library(renderer_warnings INTERFACE)
if(MSVC)
    target_compile_options(renderer_warnings INTERFACE /W4)
else()
    target_compile_options(renderer_warnings INTERFACE -Wall -Wextra -Wpedantic)
endif()

add_library(renderer_core STATIC
    src/Error.cpp
    src/Log.cpp
    src/ModelList.cpp
    src/ObjMeshData.cpp
    src/ObjPaths.cpp
    src/Paths.cpp
    src/Rotation.cpp
    src/Settings.cpp
    src/headers/Error.hpp
    src/headers/Log.hpp
    src/headers/ModelList.hpp
    src/headers/ObjMeshData.hpp
    src/headers/ObjPaths.hpp
    src/headers/Paths.hpp
    src/headers/Rotation.hpp
    src/headers/Settings.hpp
)
target_include_directories(renderer_core PUBLIC src/headers)
target_link_libraries(renderer_core PUBLIC
    nlohmann_json::nlohmann_json
    renderer_warnings
)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_compile_definitions(imgui PRIVATE
    GLFW_INCLUDE_NONE
    IMGUI_IMPL_OPENGL_LOADER_GLAD
)
target_link_libraries(imgui PUBLIC glfw glad renderer_warnings)

# ---------------------------------------------------------------------------
# The program
# ---------------------------------------------------------------------------
# List sources by hand. Globbing hides new files from CMake until you
# reconfigure, and it makes "what is in this build?" harder to answer.
add_executable(OpenGLRenderer
    src/main.cpp
    src/ObjMesh.cpp
    src/RendererUi.cpp
    src/Shader.cpp
    src/Window.cpp
    src/headers/ObjMesh.hpp
    src/headers/RendererUi.hpp
    src/headers/Shader.hpp
    src/headers/Window.hpp
)

if(WIN32)
    set_target_properties(OpenGLRenderer PROPERTIES
        WIN32_EXECUTABLE $<CONFIG:Release>)
endif()

target_include_directories(OpenGLRenderer PRIVATE src/headers)
target_link_libraries(OpenGLRenderer PRIVATE
    renderer_core
    renderer_warnings
    imgui
    glad
    glfw
    nlohmann_json::nlohmann_json
    tinyobjloader
)

target_compile_definitions(OpenGLRenderer PRIVATE
    OPENGLRENDERER_VERSION="${PROJECT_VERSION}"
)

# MinGW: link the C++ runtime into the exe so it starts on PCs without MSYS2.
if(MINGW)
    target_link_options(OpenGLRenderer PRIVATE -static)
endif()

if(BUILD_TESTING)
    add_executable(settings_tests
        tests/SettingsTests.cpp
    )
    target_link_libraries(settings_tests PRIVATE renderer_core renderer_warnings)

    add_test(NAME settings COMMAND settings_tests)

    add_executable(obj_path_tests tests/ObjPathTests.cpp)
    target_link_libraries(obj_path_tests PRIVATE renderer_core renderer_warnings)
    add_test(NAME obj_path COMMAND obj_path_tests)

    add_executable(renderer_core_tests tests/RendererCoreTests.cpp)
    target_link_libraries(renderer_core_tests PRIVATE renderer_core renderer_warnings)
    add_test(NAME renderer_core COMMAND renderer_core_tests)
endif()

# ---------------------------------------------------------------------------
# Assets: copy them next to the executable on every build. This keeps shader
# and configuration edits in sync even when no C++ source has changed.
# ---------------------------------------------------------------------------
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:OpenGLRenderer>/assets"
    COMMENT "Synchronizing assets next to the executable"
)
add_dependencies(OpenGLRenderer copy_assets)

# ---------------------------------------------------------------------------
# Install + release packaging
#   cmake --preset release
#   cmake --build --preset release --target package_release
# Output: release/<version>_<date>/OpenGLRenderer-<version>/ and a .zip of it
# ---------------------------------------------------------------------------
#
# Everything is tagged COMPONENT Runtime, and package_release installs only that
# component. Dependencies fetched with FetchContent bring their own install
# rules (tinyobjloader adds include/, lib/ and share/), and those rules have no
# component, so they stay out of the release.
install(TARGETS OpenGLRenderer RUNTIME DESTINATION . COMPONENT Runtime)
install(DIRECTORY assets DESTINATION . COMPONENT Runtime)
install(FILES LICENSE README.md THIRD_PARTY_LICENSES.txt DESTINATION . COMPONENT Runtime)

string(TOLOWER "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" RELEASE_PLATFORM)

add_custom_target(package_release
    COMMAND ${CMAKE_COMMAND}
            -DVERSION=${PROJECT_VERSION}
            -DPLATFORM=${RELEASE_PLATFORM}
            -DEXECUTABLE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}
            -DSOURCE_DIR=${PROJECT_SOURCE_DIR}
            -DBINARY_DIR=${PROJECT_BINARY_DIR}
            -DPACKAGE_PATH_FILE=${PROJECT_BINARY_DIR}/release-package-path.txt
            -P ${PROJECT_SOURCE_DIR}/cmake/MakeRelease.cmake
    DEPENDS OpenGLRenderer
    COMMENT "Packaging release ${PROJECT_VERSION}"
    VERBATIM
)
