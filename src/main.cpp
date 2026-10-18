#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Error.hpp"
#include "Log.hpp"
#include "ObjMesh.hpp"
#include "Paths.hpp"
#include "Settings.hpp"
#include "Shader.hpp"
#include "Window.hpp"

#include <cmath>
#include <cstdio>
#include <system_error>

#if defined(_WIN32) && defined(NDEBUG)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace {
struct RenderState {
    GLuint shaderProgram;
    const ObjMesh* mesh;
};

void drawFrame(GLFWwindow* window, const RenderState& state)
{
    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    if (framebufferWidth <= 0 || framebufferHeight <= 0)
        return;

    glClearColor(0.08f, 0.12f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float aspect = static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
    constexpr float focalLength = 2.41421356f;
    constexpr float nearPlane = 0.1f;
    constexpr float farPlane = 20.0f;
    const float projection[] = {
        focalLength / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, focalLength, 0.0f, 0.0f,
        0.0f, 0.0f, (farPlane + nearPlane) / (nearPlane - farPlane), -1.0f,
        0.0f, 0.0f, (2.0f * farPlane * nearPlane) / (nearPlane - farPlane), 0.0f
    };
    constexpr float view[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, -9.0f, 1.0f
    };
    const float rotation = static_cast<float>(glfwGetTime()) * 0.35f;
    const float cosine = std::cos(rotation);
    const float sine = std::sin(rotation);
    const float model[] = {
        cosine, 0.0f, -sine, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sine, 0.0f, cosine, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    glUseProgram(state.shaderProgram);
    setUniformMatrix4(state.shaderProgram, "model", model);
    setUniformMatrix4(state.shaderProgram, "view", view);
    setUniformMatrix4(state.shaderProgram, "projection", projection);
    state.mesh->draw();
    glfwSwapBuffers(window);
}

void redrawWindow(GLFWwindow* window)
{
    const auto* state = static_cast<const RenderState*>(glfwGetWindowUserPointer(window));
    if (state != nullptr)
        drawFrame(window, *state);
}

#if defined(_WIN32) && defined(NDEBUG)
void showStartupError(const char* message)
{
    MessageBoxA(nullptr, message, "OpenGL Renderer startup error", MB_OK | MB_ICONERROR);
}
#endif
}

int main()
{
#if defined(_WIN32) && defined(NDEBUG)
    bool fileLoggingInitialized = false;
    try {
        const std::filesystem::path logDirectory = userDataDir();
        std::error_code error;
        std::filesystem::create_directories(logDirectory, error);
        if (!error) {
            const std::string logPath = (logDirectory / "openglrenderer.log").string();
            const auto redirectStream = [&](FILE* stream, const char* mode) {
                FILE* redirected = nullptr;
                return freopen_s(&redirected, logPath.c_str(), mode, stream) == 0 && redirected != nullptr;
            };
            const bool errorRedirected = redirectStream(stderr, "w");
            const bool outputRedirected = redirectStream(stdout, errorRedirected ? "a" : "w");
            fileLoggingInitialized = errorRedirected && outputRedirected;
        }
    } catch (...) {
    }
#endif

    try {
        const Settings settings = loadSettings(userSettingsPath().string());

        const GlfwSession glfw;
        GLFWwindow* window = createOpenGLWindow(
            settings.title,
            settings.width,
            settings.height,
            settings.vSync);

        if (settings.wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        Log::info(
            "OpenGL version: {}",
            reinterpret_cast<const char*>(glGetString(GL_VERSION)));
        Log::info("OpenGL Renderer {}", OPENGLRENDERER_VERSION);

        const GLuint shaderProgram = createShaderProgram(
            assetPath("shaders/vertexShader.vert"),
            assetPath("shaders/fragmentShader.frag"));

        ObjMesh cube(assetPath("meshes/cube/cube.obj"));
        glEnable(GL_DEPTH_TEST);

        RenderState renderState{shaderProgram, &cube};
        glfwSetWindowUserPointer(window, &renderState);
        glfwSetWindowRefreshCallback(window, redrawWindow);

        while (!glfwWindowShouldClose(window)) {
            processInput(window);
            drawFrame(window, renderState);
            glfwPollEvents();
        }

        glfwSetWindowRefreshCallback(window, nullptr);
        glfwSetWindowUserPointer(window, nullptr);
        deleteShaderProgram(shaderProgram);

        // The window is deliberately not destroyed here. Destroying it also
        // destroys its OpenGL context, and `cube` still has to delete its
        // buffers when it goes out of scope after this return. `glfw` is
        // declared first, so it is destroyed last, and glfwTerminate()
        // closes the window once every GL object is gone.
        return 0;
    } catch (const RendererError& e) {
        Log::error(e.what());
#if defined(_WIN32) && defined(NDEBUG)
        if (!fileLoggingInitialized)
            showStartupError(e.what());
#endif
        return exitCode(e.category());
    } catch (const std::exception& e) {
        Log::error(e.what());
#if defined(_WIN32) && defined(NDEBUG)
        if (!fileLoggingInitialized)
            showStartupError(e.what());
#endif
        return 1;
    }
}
