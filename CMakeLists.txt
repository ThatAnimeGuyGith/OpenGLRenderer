cmake_minimum_required(VERSION 3.24)

# The version lives here and nowhere else. The release script reads it.
project(OpenGLRenderer VERSION 0.1.0 LANGUAGES C CXX)

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
FetchContent_MakeAvailable(glfw nlohmann_json)

# glad is generated code, so it is committed in third_party/glad.
add_library(glad STATIC third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC third_party/glad/include)
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})   # glad uses dlopen on Linux

# ---------------------------------------------------------------------------
# The program
# ---------------------------------------------------------------------------
# List sources by hand. Globbing hides new files from CMake until you
# reconfigure, and it makes "what is in this build?" harder to answer.
add_executable(OpenGLRenderer
    src/main.cpp
    src/Error.cpp
    src/Log.cpp
    src/Paths.cpp
    src/Settings.cpp
    src/Shader.cpp
    src/Window.cpp
)

target_include_directories(OpenGLRenderer PRIVATE src)
target_link_libraries(OpenGLRenderer PRIVATE glad glfw nlohmann_json::nlohmann_json)

target_compile_definitions(OpenGLRenderer PRIVATE
    OPENGLRENDERER_VERSION="${PROJECT_VERSION}"
)

if(MSVC)
    target_compile_options(OpenGLRenderer PRIVATE /W4)
else()
    target_compile_options(OpenGLRenderer PRIVATE -Wall -Wextra -Wpedantic)
endif()

# MinGW: link the C++ runtime into the exe so it starts on PCs without MSYS2.
if(MINGW)
    target_link_options(OpenGLRenderer PRIVATE -static)
endif()

# ---------------------------------------------------------------------------
# Assets: make them appear next to the executable after every build.
# Linux uses a symlink, so editing a shader needs no rebuild. Windows copies,
# because symlinks there need special privileges.
# ---------------------------------------------------------------------------
if(UNIX)
    set(ASSET_COMMAND create_symlink)
else()
    set(ASSET_COMMAND copy_directory)
endif()

add_custom_command(TARGET OpenGLRenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E ${ASSET_COMMAND}
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:OpenGLRenderer>/assets"
    COMMENT "Placing assets next to the executable"
)

# ---------------------------------------------------------------------------
# Install + release packaging
#   cmake --preset release
#   cmake --build --preset release --target package_release
# Output: release/<version>_<date>/OpenGLRenderer-<version>/ and a .zip of it
# ---------------------------------------------------------------------------
install(TARGETS OpenGLRenderer RUNTIME DESTINATION .)
install(DIRECTORY assets DESTINATION .)
install(FILES LICENSE README.md DESTINATION .)

string(TOLOWER "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" RELEASE_PLATFORM)

add_custom_target(package_release
    COMMAND ${CMAKE_COMMAND}
            -DVERSION=${PROJECT_VERSION}
            -DPLATFORM=${RELEASE_PLATFORM}
            -DSOURCE_DIR=${CMAKE_SOURCE_DIR}
            -DBINARY_DIR=${CMAKE_BINARY_DIR}
            -P ${CMAKE_SOURCE_DIR}/cmake/MakeRelease.cmake
    DEPENDS OpenGLRenderer
    COMMENT "Packaging release ${PROJECT_VERSION}"
    VERBATIM
)
