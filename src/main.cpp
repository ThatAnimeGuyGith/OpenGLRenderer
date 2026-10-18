#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Error.hpp"
#include "Log.hpp"
#include "ModelList.hpp"
#include "ObjMesh.hpp"
#include "Paths.hpp"
#include "RendererUi.hpp"
#include "Rotation.hpp"
#include "Settings.hpp"
#include "Shader.hpp"
#include "Window.hpp"

#include <cmath>
#include <cstdio>
#include <memory>
#include <string>
#include <system_error>

#if defined(_WIN32) && defined(NDEBUG)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace {
struct RenderState {
    const ShaderProgram* shaderProgram;
    const ObjMesh* mesh;
    const RotationState* rotation;
    const LightingSettings* lighting;
};

void drawScene(GLFWwindow* window, const RenderState& state)
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
    const float cosine = std::cos(state.rotation->angleRadians);
    const float sine = std::sin(state.rotation->angleRadians);
    const float model[] = {
        cosine, 0.0f, -sine, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sine, 0.0f, cosine, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    state.shaderProgram->use();
    state.shaderProgram->setMatrix4("model", model);
    state.shaderProgram->setMatrix4("view", view);
    state.shaderProgram->setMatrix4("projection", projection);
    state.shaderProgram->setVec3("baseColor", state.lighting->baseColor);
    state.shaderProgram->setVec3("lightDirection", state.lighting->lightDirection);
    state.shaderProgram->setFloat("ambientStrength", state.lighting->ambientStrength);
    state.shaderProgram->setFloat("diffuseStrength", state.lighting->diffuseStrength);
    state.mesh->draw();
}

void redrawWindow(GLFWwindow* window)
{
    const auto* state = static_cast<const RenderState*>(glfwGetWindowUserPointer(window));
    if (state != nullptr) {
        drawScene(window, *state);
        glfwSwapBuffers(window);
    }
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

        ShaderProgram shaderProgram(
            assetPath("shaders/vertexShader.vert"),
            assetPath("shaders/fragmentShader.frag"));

        int selectedModel = 0;
        std::unique_ptr<ObjMesh> mesh = std::make_unique<ObjMesh>(
            assetPath(builtInModels()[static_cast<std::size_t>(selectedModel)].assetPath));
        RotationState rotation;
        LightingSettings lighting;
        RendererUi ui(window, userDataDir() / "imgui.ini");
        glEnable(GL_DEPTH_TEST);

        RenderState renderState{&shaderProgram, mesh.get(), &rotation, &lighting};
        glfwSetWindowUserPointer(window, &renderState);
        glfwSetWindowRefreshCallback(window, redrawWindow);

        float previousTime = static_cast<float>(glfwGetTime());
        std::string modelLoadError;
        while (!glfwWindowShouldClose(window)) {
            const float currentTime = static_cast<float>(glfwGetTime());
            advanceRotation(rotation, currentTime - previousTime);
            previousTime = currentTime;
            processInput(window);
            drawScene(window, renderState);
            ui.beginFrame();
            const int previousModel = selectedModel;
            if (ui.draw(selectedModel, rotation, lighting, modelLoadError)) {
                try {
                    const auto& model = builtInModels()[static_cast<std::size_t>(selectedModel)];
                    std::unique_ptr<ObjMesh> replacement = std::make_unique<ObjMesh>(assetPath(model.assetPath));
                    mesh = std::move(replacement);
                    renderState.mesh = mesh.get();
                    modelLoadError.clear();
                } catch (const std::exception& error) {
                    modelLoadError = error.what();
                    Log::error(modelLoadError);
                    selectedModel = previousModel;
                }
            }
            ui.render();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwSetWindowRefreshCallback(window, nullptr);
        glfwSetWindowUserPointer(window, nullptr);
        // The window is deliberately not destroyed here. Destroying it also
        // destroys its OpenGL context, and `mesh` still has to delete its
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
